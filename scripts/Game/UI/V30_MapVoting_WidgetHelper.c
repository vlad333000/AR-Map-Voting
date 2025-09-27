class V30_MapVoting_WidgetHelperT<Class T> {
    protected /*private*/ Widget m_Widget;

    /*sealed*/ void V30_MapVoting_WidgetHelperT(notnull Widget widget) {
        m_Widget = widget;
    };
	
	/*sealed*/ void SetWidget(notnull Widget widget) {
		m_Widget = widget;
	};

    /*sealed*/ T GetParent() {
        return GetParent(m_Widget);
    };

    /*sealed*/ static T GetParent(notnull Widget widget) {
        return T.Cast(widget.GetParent());
    };

    /*sealed*/ T GetChildren() {
        return GetChildren(m_Widget);
    };

    /*sealed*/ static T GetChildren(notnull Widget widget) {
        return T.Cast(widget.GetChildren());
    };

    /*sealed*/ T GetSibling() {
        return GetSibling(m_Widget);
    };

    /*sealed*/ static T GetSibling(notnull Widget widget) {
        return T.Cast(widget.GetSibling());
    };

    /*sealed*/ T FindWidget(string pathname) {
        return FindWidget(m_Widget, pathname);
    };

    /*sealed*/ static T FindWidget(notnull Widget widget, string pathname) {
        return T.Cast(widget.FindWidget(pathname));
    };

    /*sealed*/ T FindAnyWidget(string pathname) {
        return FindAnyWidget(m_Widget, pathname);
    };

    /*sealed*/ static T FindAnyWidget(notnull Widget widget, string pathname) {
        return T.Cast(widget.FindAnyWidget(pathname));
    };

    /*sealed*/ T FindAnyWidgetById(int user_id) {
        return FindAnyWidgetById(m_Widget, user_id);
    };

    /*sealed*/ static T FindAnyWidgetById(notnull Widget widget, int user_id) {
        return T.Cast(widget.FindAnyWidgetById(user_id));
    };
};

class V30_MapVoting_WidgetHandlerHelperT<Class T> {
    protected /*private*/ Widget m_Widget;

    /*sealed*/ void V30_MapVoting_WidgetHandlerHelperT(notnull Widget widget) {
        m_Widget = widget;
    };
	
	/*sealed*/ void SetWidget(notnull Widget widget) {
		m_Widget = widget;
	};

    /*sealed*/ T GetHandler(int index) {
        return GetHandler(m_Widget, index);
    };

    /*sealed*/ static T GetHandler(notnull Widget widget, int index) {
        return T.Cast(widget.GetHandler(index));
    };

    /*sealed*/ T FindHandler() {
        return FindHandler(m_Widget);
    };

    /*sealed*/ static T FindHandler(notnull Widget widget) {
        return T.Cast(widget.FindHandler(T));
    };

    /*sealed*/ T FindChildrenHandler(bool recursive = false) {
        return FindChildrenHandler(m_Widget, recursive);
    };

	/*sealed*/ static T FindChildrenHandler(notnull Widget widget, bool recursive = false) {
        Widget children = widget.GetChildren();
        while (children) {
            auto handler = FindHandler(children);
            if (!handler && recursive)
                handler = FindChildrenHandler(children, true);
            if (handler)
                return handler;
            children = children.GetSibling();
        };
        return null;
    };

    /*sealed*/ void FindChildrenHandlers(notnull array<T> handlers, bool recursive = false, bool recursiveHandlers = false) {
        return FindChildrenHandlers(m_Widget, handlers, recursive, recursiveHandlers);
    };

    /*sealed*/ static void FindChildrenHandlers(notnull Widget widget, notnull array<T> handlers, bool recursive = false, bool recursiveHandlers = false) {
        Widget children = widget.GetChildren();
        while (children) {
            auto handler = FindHandler(children);
            if (handler) {
                handlers.Insert(handler);
                if (recursiveHandlers)
                    FindChildrenHandlers(children, handlers, recursive, true);
            }
            else if (recursive)
                FindChildrenHandlers(children, handlers, true, recursiveHandlers);
            children = children.GetSibling();
        };
    };
};
