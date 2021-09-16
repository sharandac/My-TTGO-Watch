#ifdef NATIVE_64BIT
        #include <stdio.h>
        #include <stdlib.h>
        #include <errno.h>
        #include <linux/unistd.h>       /* for _syscallX macros/related stuff */
        #include <linux/kernel.h>       /* for struct sysinfo */
        #include <sys/sysinfo.h>
        #include "millis.h"

        long millis( void ) {
            struct sysinfo s_info;
            int error = sysinfo(&s_info);
            if(error != 0) {
                printf("code error = %d\n", error);
            }
            return s_info.uptime;        
        }
#endif