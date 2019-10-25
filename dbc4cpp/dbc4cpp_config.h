#ifndef DBC4CPP_CONFIG_H
#define DBC4CPP_CONFIG_H

#ifdef USE_DBC4CPP_NAMESPACE
    #define BEGIN_DBC4CPP_NAMESPACE namespace {
    #define END_DBC4CPP_NAMESPACE }
#else
    #define BEGIN_DBC4CPP_NAMESPACE
    #define END_DBC4CPP_NAMESPACE
#endif

#endif // DBC4CPP_CONFIG_H
