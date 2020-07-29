#ifndef _RESOLVE_OWM_ICON_H
    #define _RESOLVE_OWM_ICON_H

    struct owm_icon {
        char iconname[8];
        const void *icon;
    };

    const void * resolve_owm_icon( char * iconname );

#endif // _RESOLVE_OWM_ICON_H