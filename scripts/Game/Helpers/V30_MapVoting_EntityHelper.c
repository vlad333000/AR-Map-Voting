class V30_MapVoting_ComponentHelper<Class T> {
    static T FindComponent(notnull IEntity owner) {
        return T.Cast(owner.FindComponent(T));
    };
};
