
#ifndef DWM_EXTENIONS_SLOCK_SLOCK_H
#define DWM_EXTENIONS_SLOCK_SLOCK_H

#ifdef HAVE_EXTENSION_LOCK
#error                                                                         \
    "HAVE_EXTENSION_LOCK has already been defined, only one lock extension is supported!"
#endif // HAVE_EXTENSION_LOCK

#define HAVE_EXTENSION_LOCK 1

#define DWM_EXTENSIONS_LOCK_CMD ((const char*[]){"slock", NULL})
#define DWM_EXTENSIONS_SUSPEND_LOCK_CMD                                        \
    ((const char*[]){"slock", "systemctl", "suspend", "-i", NULL})

#endif // !DWM_EXTENIONS_SLOCK_SLOCK_H
