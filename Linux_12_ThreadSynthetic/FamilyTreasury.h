#ifndef __FAMILY_TREASURY_H__
#define __FAMILY_TREASURY_H__

#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <vector>

using UINT32 = unsigned int;

struct FamilyAccount {
    int balance;           // 余额
    pthread_mutex_t mutex; // 只能一个人动钱！
    pthread_cond_t cond;   // 没钱我就哭给你看！
};

// --- 2. 审计账本 (用 RWLock，因为读多写少) ---
struct AuditLedger {
    std::vector<std::string> logs; // 存流水的
    pthread_rwlock_t rwlock;       // 保护 logs
};

// --- 3. 老公状态 (极短操作，用 Spinlock) ---
enum HusbandStatus {IDLE, WORKING, RESTING};
struct HusbandState {
    HusbandStatus status;
    pthread_spinlock_t spinlock;
};

class FamilyAccountRAII {
    FamilyAccount* account_;
    AuditLedger* ledger_;
    HusbandState* husband_state_;
public:
    FamilyAccountRAII(FamilyAccount* &account, AuditLedger* &ledger, HusbandState* &husband_state)     
    {
        account = new FamilyAccount();
        ledger = new AuditLedger();
        husband_state = new HusbandState();

        pthread_mutex_init(&account->mutex, nullptr);
        pthread_cond_init(&account->cond, nullptr);
        pthread_rwlock_init(&ledger->rwlock, nullptr);
        pthread_spin_init(&husband_state->spinlock, PTHREAD_PROCESS_PRIVATE);

        account->balance = 0;
        husband_state->status = IDLE;

        account_ = account;
        ledger_ = ledger;
        husband_state_ = husband_state;
    }
    
    void accountAfterHandle()
    {
        if (account_ == nullptr) {
            return;
        }
        pthread_mutex_unlock(&account_->mutex);
        pthread_mutex_destroy(&account_->mutex);
        pthread_cond_destroy(&account_->cond);
        delete account_;
        account_ = nullptr;
    }

    void ledgerAfterHandle()
    {
        if (ledger_ == nullptr) {
            return;
        }
        pthread_rwlock_unlock(&ledger_->rwlock);
        pthread_rwlock_destroy(&ledger_->rwlock);
        delete ledger_;
        ledger_ = nullptr;

    }

    void husbandStatusAfterHandle()
    {
        if (husband_state_ == nullptr) {
            return;
        }
        pthread_spin_unlock(&husband_state_->spinlock);
        pthread_spin_destroy(&husband_state_->spinlock);
        delete  husband_state_;
        husband_state_ = nullptr;
    }

    ~FamilyAccountRAII() {
        accountAfterHandle();
        ledgerAfterHandle();
        husbandStatusAfterHandle();
    }
};

#endif // __FAMILY_TREASURY_H__