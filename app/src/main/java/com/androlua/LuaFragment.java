package com.androlua;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;

import com.luajava.LuaException;
import com.luajava.LuaObject;
import com.luajava.LuaTable;

public class LuaFragment extends Fragment {

    private LuaTable mLayout = null;
    private LuaObject mLoadLayout = null;
    private View mView;

    public LuaFragment() {
        super();
    }

    public LuaFragment(LuaTable layout) throws LuaException {
        super();
        mLayout = layout;
        mLoadLayout = (LuaObject) (layout.getLuaState().getLuaObject("require").call("loadlayout"));
    }

    public LuaFragment(View layout) {
        super();
        mView = layout;
    }

    public void setLayout(LuaTable layout) throws LuaException {
        mLayout = layout;
        mLoadLayout = (LuaObject) (layout.getLuaState().getLuaObject("require").call("loadlayout"));
    }

    public void setLayout(View layout) {
        mView = layout;
    }

    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        try {
            if (mView != null)
                return mView;
            if (mLayout != null)
                return (View) mLoadLayout.call(mLayout);
            return new TextView(getActivity());
        } catch (Exception e) {
            throw new IllegalArgumentException(e.getMessage());
        }
    }
}
