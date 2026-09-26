/*
Used to get VS Code Intellisense working correctly for header definitions
protected by compiler-specific feature flags.
*/
#ifdef __GNUC__
#define _ATTRIBUTE(attrs) __attribute__(attrs)
#else
#define _ATTRIBUTE(attrs)
#endif
