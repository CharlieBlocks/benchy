#ifndef _BENCHY_DISPATCHER_H
#define _BENCHY_DISPATCHER_H

#include <queue>
#include <thread>
#include <mutex>
#include <memory>

enum class JobStatus {
    Waiting,
    Running,
    Done
};

class Job {
public:
    Job()   = default;
    ~Job()  = default;

    /*
    The virtual method that is called when the job is executed.
    This should be ovrriden by the class inheriting Job.
    */
    virtual void proc() = 0;


    inline JobStatus state() { 
        return m_status.load(std::memory_order::memory_order_seq_cst);
    }

    inline bool is_done() { 
        std::lock_guard<std::mutex> _(m_lock);
        return m_status.load(std::memory_order::memory_order_seq_cst) == JobStatus::Done;
    }

    inline void wait() { 
        std::unique_lock<std::mutex> lock(m_lock);
        m_condVar.wait(lock, [&]() { return m_status.load(std::memory_order::memory_order_seq_cst) == JobStatus::Done; });
        lock.unlock();
    }


protected:
    void execute();

    inline std::unique_lock<std::mutex> data_lock() { return std::unique_lock<std::mutex>(m_lock); }

    inline void update_status(JobStatus status) {
        m_status.store(status, std::memory_order::memory_order_seq_cst);
    }

    // Allow JobDispatcher to call the execute method
    friend class JobDispatcher;

private:
    std::atomic<JobStatus>  m_status = JobStatus::Waiting;
    std::mutex              m_lock;
    std::condition_variable m_condVar;
};


class JobDispatcher {
public:
    JobDispatcher(int poolSize);
    ~JobDispatcher();

    void queue_job(std::shared_ptr<Job> job);
    
    void clear();


private:
    void thread_proc();

private:
    bool m_isRunning;
    int m_poolSize;
    std::unique_ptr<std::thread[]> m_threadPool;

    std::queue<std::shared_ptr<Job>>       m_jobQueue;

    std::condition_variable m_conditionVar;
    std::mutex              m_queueMutex;

};

#endif // _BENCHY_DISPATCHER_H