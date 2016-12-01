_Pragma ("once");

#include <mutex>
#include <atomic>
#include <condition_variable>

namespace rbl {
    template<typename Mutex>
    class RBLock {
    public:
        RBLock() : writer{false}, pendingWriter{0}, reader{0} {}

        void readLock() {
            std::unique_lock<Mutex> guard(mut);
            // The priority of writer is higher than reader
            readProcess.wait(guard, [&] { return !writer && !pendingWriter; });
            reader++;
        }

        void writeLock() {
            std::unique_lock<Mutex> guard(mut);
            pendingWriter++;
            writeProcess.wait(guard, [&] { return !writer && !reader; });
            writer = true;
            pendingWriter--;
        }

        void unlock() {
            std::lock_guard<Mutex> guard(mut);
            // writer = true->false; reader = 0; pendingWriter >= 0;
            // OR: writer = false; --reader >= 0; pendingWriter >= 0;
            if (writer) writer = false;
            else if (reader) reader--;
            // again, writer is higher than reader
            // (requests of reader and writer are all coming when writing)
            if (!reader && pendingWriter)
                writeProcess.notify_one();
            else // reader > 0 || pendingWriter == 0
                readProcess.notify_all();
        }

    private:
        std::atomic<bool> writer;
        std::atomic<int> pendingWriter;
        std::atomic<int> reader; //the number of reader processing
        Mutex mut;
        std::condition_variable writeProcess, readProcess;
    };

    template<typename RBL>
    class read_guard {
    public:
        read_guard(RBL &l) : lk{l} { lk.readLock(); }

        ~read_guard() { lk.unlock(); }

    private:
        RBL &lk;
    };

    template<typename RBL>
    class write_guard {
    public:
        write_guard(RBL &l) : lk{l} { lk.writeLock(); }

        ~write_guard() { lk.unlock(); }

    private:
        RBL &lk;
    };
}