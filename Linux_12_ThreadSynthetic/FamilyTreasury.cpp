#include "FamilyTreasury.h"
#include <iomanip>
#include <ios>
#include <pthread.h>
#include <sstream>
#include <unistd.h>
#include <vector>

using namespace std;

FamilyAccount* g_family_account = nullptr;
AuditLedger* g_audit_ledger = nullptr;
HusbandState* g_husband_state = nullptr;
ostringstream g_log_stream;

void handover_work_balance()
{
    UINT32 amount = rand() % 2000 + 12000;

    pthread_mutex_lock(&g_family_account->mutex);
    g_family_account->balance += amount;
    pthread_mutex_unlock(&g_family_account->mutex);

    pthread_rwlock_wrlock(&g_audit_ledger->rwlock);
    g_log_stream << "😭🧎‍♂️🤲💴💴 老公辛苦搬砖，上缴工资: " << amount
        << " 元，当前家庭总额: " << g_family_account->balance << " 元。" << endl;;
    g_audit_ledger->logs.push_back(g_log_stream.str());
    g_log_stream.clear();
    g_log_stream.str("");
    pthread_rwlock_unlock(&g_audit_ledger->rwlock);
    
}

void surrender_private_saving()
{
    UINT32 amount = rand() % 500 + 200;

    pthread_mutex_lock(&g_family_account->mutex);
    g_family_account->balance += amount;
    pthread_mutex_unlock(&g_family_account->mutex);

    pthread_rwlock_wrlock(&g_audit_ledger->rwlock);
    g_log_stream << "😭🧎‍♂️🤲💴💴 老公仅剩的私房钱: " << amount
        << " 被老婆发现，被拧耳朵全部上交，当前家庭总额: " << g_family_account->balance << " 元。" << endl;;
    g_audit_ledger->logs.push_back(g_log_stream.str());
    g_log_stream.clear();
    g_log_stream.str("");
    pthread_rwlock_unlock(&g_audit_ledger->rwlock);
    
}

void hangover_work_bonus()
{
    UINT32 amount = rand() % 3000 + 18000;

    pthread_mutex_lock(&g_family_account->mutex);
    g_family_account->balance += amount;
    pthread_mutex_unlock(&g_family_account->mutex);

    pthread_rwlock_wrlock(&g_audit_ledger->rwlock);
    g_log_stream << "😭🧎‍♂️🤲💴💴 老公拿到的技术奖金: " << amount
        << " 被老婆注意，接收一个亲亲后全部上交，当前家庭总额: " << g_family_account->balance << " 元。" << endl;;
    g_audit_ledger->logs.push_back(g_log_stream.str());
    g_log_stream.clear();
    g_log_stream.str("");
    pthread_rwlock_unlock(&g_audit_ledger->rwlock);
    
}

using moneyHangoverFunc = void(*)();

void* husband_labor_routine(void* arg) // 老公干活线程
{
    vector<moneyHangoverFunc>  money_hangover_funcs = {
        handover_work_balance,
        surrender_private_saving,
        hangover_work_bonus
    };
    size_t index = 0;
    while (true) {
        pthread_spin_lock(&g_husband_state->spinlock);
        g_husband_state->status = WORKING;
        pthread_spin_unlock(&g_husband_state->spinlock);

        index = rand() % money_hangover_funcs.size();
        money_hangover_funcs[index]();

        pthread_spin_lock(&g_husband_state->spinlock);
        g_husband_state->status = RESTING;
        pthread_spin_unlock(&g_husband_state->spinlock);

        pthread_cond_signal(&g_family_account->cond);
        usleep(1000000);
    }
}

void* wife_shopping_routine(void* arg) // 老婆消费线程
{
    while (true) 
    {
        UINT32 shopping_amount = rand() % 10000 + 800;
        pthread_mutex_lock(&g_family_account->mutex);
        while (g_family_account->balance < shopping_amount) {
            cout << "🛍️👗👠 老婆想购物，当前余额 " << g_family_account->balance
                 << " 元，不够购物需求 " << shopping_amount << " 元，催老公去干活！" << endl;
            pthread_cond_wait(&g_family_account->cond, &g_family_account->mutex);
        }
        g_family_account->balance -= shopping_amount;
        pthread_mutex_unlock(&g_family_account->mutex);

        pthread_rwlock_wrlock(&g_audit_ledger->rwlock);
        g_log_stream << "🛍️👗👠🧧💑 老婆开心购物花钱: " << shopping_amount
            << " 元，当前家庭总额: " << g_family_account->balance << endl;
        g_audit_ledger->logs.push_back(g_log_stream.str());
        g_log_stream.clear();
        g_log_stream.str("");
        pthread_rwlock_unlock(&g_audit_ledger->rwlock);
    }
}

void* cfo_audit_routine(void* arg) // CFO审核线程
{
    UINT32 log_index = 0;
    HusbandStatus current_status = IDLE;
    while (true) {
        usleep((2 + rand() % 3) * 1000000);
        pthread_spin_lock(&g_husband_state->spinlock);
        current_status = g_husband_state->status;
        pthread_spin_unlock(&g_husband_state->spinlock);
        cout << "🕵️‍♀️ CFO暗中观察：老公当前状态是 [" 
            << (current_status == WORKING ? "搬砖中" : "偷懒中") << "]" << endl;

        pthread_rwlock_rdlock(&g_audit_ledger->rwlock);
        if (log_index == g_audit_ledger->logs.size()) {
            pthread_rwlock_unlock(&g_audit_ledger->rwlock);
            continue;
        }
        cout << "📋 CFO 审核家庭账本，当前流水记录如下：" << endl;
        for (; log_index < g_audit_ledger->logs.size(); ++log_index) {
            cout << "["  << setw(3) << left << log_index << "] " << g_audit_ledger->logs[log_index];
        }
        pthread_rwlock_unlock(&g_audit_ledger->rwlock);
    }
}

int main()
{
    FamilyAccountRAII wife_is_my_beloved_forever(g_family_account, g_audit_ledger, g_husband_state);
    vector<pthread_t> pths(6, 0);
    pthread_create(&pths[0], nullptr, wife_shopping_routine, nullptr);
    pthread_create(&pths[1], nullptr, wife_shopping_routine, nullptr);
    pthread_create(&pths[2], nullptr, wife_shopping_routine, nullptr);
    pthread_create(&pths[3], nullptr, husband_labor_routine, nullptr);
    pthread_create(&pths[4], nullptr, cfo_audit_routine, nullptr);
    pthread_create(&pths[5], nullptr, cfo_audit_routine, nullptr);

    for (auto it : pths) {
        pthread_join(it, nullptr);
    }
}