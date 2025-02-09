#include "dispatcher.hpp"
#include <functional>

#include <iostream>


/* 
Job Implementation
*/
void Job::execute() {
    update_status(JobStatus::Running);
    this->proc();
    update_status(JobStatus::Done);
    m_condVar.notify_all();
}



/*
JobDispatcher Implementation
*/
JobDispatcher::JobDispatcher(int poolSize)
: m_poolSize(poolSize), m_isRunning(true) {
    // Initialise thread pool
    m_threadPool = std::unique_ptr<std::thread[]>(new std::thread[poolSize]);
    for (int i = 0; i < poolSize; ++i)
        m_threadPool[i] = std::thread(std::bind(&JobDispatcher::thread_proc, this));
}

JobDispatcher::~JobDispatcher() {
    {
        std::lock_guard<std::mutex> _(m_queueMutex);
        m_isRunning = false;
    }
    m_conditionVar.notify_all();

    for (int i = 0; i < m_poolSize; i++)
        m_threadPool[i].join();
}

void JobDispatcher::queue_job(std::shared_ptr<Job> job) {
    {
        std::lock_guard<std::mutex> _(m_queueMutex);
        m_jobQueue.push(job);
    }
    m_conditionVar.notify_one();
}

void JobDispatcher::clear() {
    std::lock_guard<std::mutex> _(m_queueMutex);
    for (int i = 0; i < m_jobQueue.size(); ++i)
        m_jobQueue.pop();
}

void JobDispatcher::thread_proc() {
    while (1) {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_conditionVar.wait(lock, [&]() { return !m_isRunning || m_jobQueue.size() > 0; });
        if (!m_isRunning) break;

        std::shared_ptr<Job> job = m_jobQueue.front();
        m_jobQueue.pop();

        lock.unlock();

        job->execute();
    }

}
