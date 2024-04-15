
#ifndef DWM_EXTENIONS_ALACRITTY_ALACRITTY_H
#define DWM_EXTENIONS_ALACRITTY_ALACRITTY_H

#ifdef HAVE_EXTENSION_TERM
#error                                                                         \
    "HAVE_EXTENSION_TERM has already been defined, only one terminal extension is supported!"
#endif // HAVE_EXTENSION_TERM

#define HAVE_EXTENSION_TERM 1

#define DWM_EXTENSIONS_TERM_CMD ((const char*[]){"alacritty", NULL})

#endif // !DWM_EXTENIONS_ALACRITTY_ALACRITTY_H
