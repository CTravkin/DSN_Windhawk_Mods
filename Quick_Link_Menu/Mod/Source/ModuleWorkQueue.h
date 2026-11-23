class ModuleWorkQueue {
public:
    bool Start() {
        const std::lock_guard guard(lock_);
        group_ = CreateThreadpoolCleanupGroup();
        if (!group_) return false;
        InitializeThreadpoolEnvironment(&environment_);
        SetThreadpoolCallbackCleanupGroup(&environment_, group_, nullptr);
        return true;
    }

    bool Submit(PTP_SIMPLE_CALLBACK callback, void* context = nullptr) {
        const std::lock_guard guard(lock_);
        return group_ && TrySubmitThreadpoolCallback(callback, context, &environment_);
    }

    void Stop() {
        PTP_CLEANUP_GROUP group;
        {
            const std::lock_guard guard(lock_);
            group = std::exchange(group_, nullptr);
        }
        if (!group) return;
        // Run queued callbacks so they can release their own context.
        CloseThreadpoolCleanupGroupMembers(group, FALSE, nullptr);
        CloseThreadpoolCleanupGroup(group);
        DestroyThreadpoolEnvironment(&environment_);
    }

private:
    std::mutex lock_;
    PTP_CLEANUP_GROUP group_{};
    TP_CALLBACK_ENVIRON environment_{};
};
