#include "Vulkan/GeneralIncludes.h"

  
struct MuticePool {
  std::string static Lock(std::string* t = new std::string) {
    internalMutices.first->lock();

    auto m = new std::mutex();
    if (*t == "") {
      failedAddingMutice:
        auto uuid = sole::uuid4().str();
        auto pair = muticePool.emplace(uuid, m);
        if (pair.second) m->lock();
        else goto failedAddingMutice;

      internalMutices.first->unlock();
      return pair.first->first;
    }; //if locked

    auto itr = muticePool.find(*t);
    if (itr == muticePool.end()) {
      randomRedo:
      auto uuid = sole::uuid4().str();
      auto pair = muticePool.emplace(uuid, m);
      if (pair.second) m->lock();
      else goto randomRedo;
    } //if itr = muticePool.end
    else {
      itr->second->lock();
    }; //else

    internalMutices.first->unlock();
    return *t;
  }; //Lock

  void static Unlock(std::string t) {
    internalMutices.second->lock();
    auto itr = muticePool.find(t);
    while (itr == muticePool.end()) {
      currentTime = std::chrono::high_resolution_clock::now();
      auto minuteCount = std::chrono::duration<float,
        std::chrono::seconds::period>().count();
      itr = muticePool.find(t);
      if (minuteCount == 1) break;
    }; //for ++i
    if (itr!=muticePool.end()) itr->second->unlock();
    internalMutices.second->unlock();
    return;
  }; //Unlock

  void static Unlock() {
    muticePool.clear();
  }; //Unlock

  private:
    static inline std::chrono::steady_clock::time_point urpreviousTime = {};
    static inline std::chrono::steady_clock::time_point currentTime = {};
  static inline 
    std::pair<std::mutex*, std::mutex*> internalMutices = 
    std::pair<std::mutex*, std::mutex*>(new std::mutex(), new std::mutex());
  static inline std::map<std::string, std::mutex*> muticePool = {};
}; //MuticePool

struct ThreadPool {
  static void FinishWork(std::vector<std::string> ids) {
    for (auto str : ids) {
      findString:
      DoWork();
      auto f = finishedQueue.find(str);
      if (f == finishedQueue.end()) goto findString;
      finishedQueue.erase(f);
    }; //for every Id
  }; //FinishWork
  
  static std::string AddWork(std::function<void()> work) {
    
    auto muticeId = MuticePool::Lock();
    uuidCheck:
    auto uuid = sole::uuid4().str();
    if (!workQueue.emplace(uuid, work).second) goto uuidCheck;

    if (threadDeque.empty())
    { auto thread = new std::thread(DoTimedWork);
      threadDeque.push_back(thread->get_id()); };

    if ((workQueue.size() > threadDeque.size() + 2)
      && (std::thread::hardware_concurrency() > threadDeque.size())) {
      auto thread = new std::thread(DoTimedWork);
      threadDeque.emplace_back(thread->get_id()); };

    MuticePool::Unlock(muticeId);
    return uuid;
  }; //AddWork

private:
  static inline std::deque<std::thread::id> threadDeque               = {};
  static inline std::map<std::string,std::function<void()>> workQueue = {};
  static inline std::map<std::string,bool> finishedQueue              = {};
  static inline std::chrono::steady_clock::time_point previousTime    = {};
  static inline std::chrono::steady_clock::time_point currentTime     = {};
  static inline uint8_t minuteCount                                   = {};
  static inline std::vector<std::string> mutices                      = {};

  static void DoTimedWork() {
    if (mutices.size() != 3) { mutices.resize(3); };

    while (!threadDeque.empty()) {

      DoWork();

      mutices[2] = MuticePool::Lock(&mutices[2]);
      if (minuteCount >= 1) {
        minuteCount = 0;
        previousTime = std::chrono::high_resolution_clock::now();
        threadDeque.pop_front();
        MuticePool::Unlock(mutices[2]);

        return;
      }; //Check Minute

      currentTime = std::chrono::high_resolution_clock::now();
      minuteCount = std::chrono::duration<float,
        std::chrono::minutes::period>(currentTime -
          previousTime).count();
      MuticePool::Unlock(mutices[2]);
    }; //While Queue is not Empty
  }; //DoTimedWork

  static void DoWork() {
    if (mutices.size() != 3) { mutices.resize(3); };
    if (workQueue.size() > 0) {
      //mutices[0] = MuticePool::Lock(&mutices[0]);
      auto func = workQueue.begin()->second;
      auto id = workQueue.begin()->first;
      workQueue.erase(workQueue.begin());
      //MuticePool::Unlock(mutices[0]);
      func();
      //mutices[1] = MuticePool::Lock(&mutices[1]);
      finishedQueue.emplace(id, true);
      previousTime = std::chrono::high_resolution_clock::now();
      //MuticePool::Unlock(mutices[1]);
    }; //Add From Queue  
  }; //DoWork

}; //ThreadFactory

