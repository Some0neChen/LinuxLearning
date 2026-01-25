#include "Harem.h"
#include <pthread.h>
#include <vector>
#include <set>

const UCHAR MAX_LOUNGE_SEATS = 4;
const UCHAR CONCUBINE_NUM = 8;
const UCHAR THE_LOARD_NUM = 1;

using namespace std;

auto cmp = [](const concubine* const &a, const concubine* const &b) -> bool {
    return a->get_glamor() < b->get_glamor();
};

deque<string> g_concubine_names = {
    "姜妩", "阮酥", "鱼幼薇", "步非烟",
    "尹雪艳", "沈惊鸿", "婠婠", "绯烟"
};
priority_queue<concubine*, deque<concubine*>, decltype(cmp)> g_harm_pq(cmp);
set<concubine*> g_ready_concubines_set;

pthread_mutex_t g_name_mutex;
pthread_mutex_t g_lounge_mutex;

pthread_cond_t g_lounge_cond;
pthread_cond_t g_lord_cond;

void* the_lord(void* args)
{
    cout << "👑 我来到了后宫..." << endl;
    concubine* consort;
    while (true) {
        pthread_mutex_lock(&g_lounge_mutex);
        while(g_harm_pq.empty()) {
            cout << "👑 💤 没人来？那我先睡会..." << endl;
            pthread_cond_wait(&g_lord_cond, &g_lounge_mutex);
        }
        cout << "👑 爱妃来啦~ 💋💃🔥❤️❤️" << endl;
        consort = g_harm_pq.top();
        g_harm_pq.pop();
        g_ready_concubines_set.erase(consort);

        pthread_mutex_unlock(&g_lounge_mutex);
        pthread_cond_broadcast(&g_lounge_cond);
        consort->enjoy_blissed();
        cout << "👑 爱妃休息去了，我也累了，先歇会儿..." << endl;
        cout << "✨ 贤者时间：回血中... (剩余排队: " << g_harm_pq.size() << ")" << endl;
        usleep(2000000);
    }
}

void* the_concubine(void* args)
{
    pthread_mutex_lock(&g_name_mutex);
    string name = g_concubine_names.front();
    g_concubine_names.pop_front();
    pthread_mutex_unlock(&g_name_mutex);

    UINT32 glamor = 90 + rand() % 10;
    concubine* consort = new concubine(name, glamor, to_string(pthread_self()));
    concubine_raii _gurad(consort);
    consort->come();

    while (true) {
        consort->dress_up();
        pthread_mutex_lock(&g_lounge_mutex);
        while (g_harm_pq.size() >= MAX_LOUNGE_SEATS) {
            consort->coquetry();
            pthread_cond_wait(&g_lounge_cond, &g_lounge_mutex);
        }
        if (!g_harm_pq.empty() && g_harm_pq.top()->get_glamor() < consort->get_glamor()) {
            consort->cut_in();
        } else {
            consort->coy();
        }
        g_harm_pq.push(consort);
        g_ready_concubines_set.insert(consort);
        pthread_cond_signal(&g_lord_cond);

        while (g_ready_concubines_set.find(consort) != g_ready_concubines_set.end()) {
            consort->wait_in_lounge();
            pthread_cond_wait(&g_lounge_cond, &g_lounge_mutex);
        }
        pthread_mutex_unlock(&g_lounge_mutex);
        consort->rest();
    }
}

int main()
{
    pthread_mutex_init(&g_name_mutex, nullptr);
    pthread_mutex_init(&g_lounge_mutex, nullptr);
    pthread_cond_init(&g_lounge_cond, nullptr);
    pthread_cond_init(&g_lord_cond, nullptr);
    vector<pthread_t> pthread_vec(THE_LOARD_NUM + CONCUBINE_NUM, 0);
    pthread_create(&pthread_vec[0], nullptr, the_lord, nullptr);
    for (int i = 1; i < THE_LOARD_NUM + CONCUBINE_NUM; ++i) {
        pthread_create(&pthread_vec[i], nullptr, the_concubine, nullptr);
        usleep(500000); // 每隔0.5秒产生一个新宠姬
    }

    for (auto it: pthread_vec) {
        pthread_join(it, nullptr);
    }
}