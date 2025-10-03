package com.androlua;

import android.os.Handler;

public final class Ticker {
    private final Handler mHandler;

    private OnTickListener mOnTickListener;

    private final Thread mThread;

    private long mPeriod = 1000;

    private boolean mEnabled = true;

    private boolean isRun = false;

    private long mLast;

    private long mOffset;


    public Ticker() {
        mHandler = new Handler(msg -> {
            if (mOnTickListener != null) mOnTickListener.onTick();
            return true;
        });
        mThread = new Thread(() -> {
            isRun = true;
            while (isRun) {
                long now = System.currentTimeMillis();
                if (!mEnabled) mLast = now - mOffset;
                if (now - mLast >= mPeriod) {
                    mLast = now;
                    mHandler.sendEmptyMessage(0);
                }
                try {
                    Thread.sleep(1);
                } catch (InterruptedException ignored) {
                }

            }
        });
    }

    public final void setPeriod(long period) {
        mLast = System.currentTimeMillis();
        mPeriod = period;
    }

    public final long getPeriod() {
        return mPeriod;
    }


    public final void setInterval(long period) {
        mLast = System.currentTimeMillis();
        mPeriod = period;
    }

    public final long getInterval() {
        return mPeriod;
    }

    public final void setEnabled(boolean enabled) {
        mEnabled = enabled;
        if (!enabled) mOffset = System.currentTimeMillis() - mLast;
    }

    public final boolean getEnabled() {
        return mEnabled;
    }

    public final void setOnTickListener(OnTickListener ltr) {
        mOnTickListener = ltr;
    }

    public final void start() {
        mThread.start();
    }

    public final void stop() {
        isRun = false;
    }

    public final boolean isRun() {
        return isRun;
    }

    public interface OnTickListener {
        void onTick();
    }
}
