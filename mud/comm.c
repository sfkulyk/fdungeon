// Copyrights (C) 1998-2003, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'

// This file contains all of the OS-dependent stuff:
// startup, signals, BSD sockets for tcp/ip, i/o, timing.

//  Game_loop ---> Read_from_descriptor ---> Read
//  Game_loop ---> Read_from_buffer
// The data flow for output is:
//  Game_loop ---> Process_Output ---> Write_to_descriptor -> Write
// The OS-dependent functions are Read_from_descriptor and Write_to_descriptor.
// -- Furey  26 Jan 1993

#include <sys/types.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#if defined( WIN32 )
#include <winsock.h>
#include "telnet.h"
const char echo_off_str[]={(char)IAC,(char)WILL,(char)TELOPT_ECHO,'\0'};
const char echo_on_str []={(char)IAC,(char)WONT,(char)TELOPT_ECHO,'\0'};
const char go_ahead_str[]={(char)IAC,(char)GA,'\0'};
#endif

#include "merc.h"
#include "interp.h"
#include "recycle.h"
#include "tables.h"
#include "olc.h"

#if     defined( WIN32 )
void    gettimeofday    args( ( struct timeval *tp, void *tzp ) );
#endif

/* Malloc debugging stuff. */
#if defined(sun)
#undef MALLOC_DEBUG
#endif

#if defined(MALLOC_DEBUG)
#include <malloc.h>
extern  int     malloc_debug    args( ( int  ) );
extern  int     malloc_verify   args( ( void ) );
#endif

/* Signal handling.
 * Apollo has a problem with __attribute(atomic) in signal.h,
 *   I dance around it. */
#if defined(apollo)
#define __attribute(x)
#endif

#if defined(unix)
#include <signal.h>
#include <execinfo.h>
#endif

#if defined(apollo)
#undef __attribute
#endif

/* Socket and TCP/IP stuff. */
#if     defined(unix)
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "telnet.h"
const   char    echo_off_str    [] = { IAC, WILL, TELOPT_ECHO, '\0' };
const   char    echo_on_str     [] = { IAC, WONT, TELOPT_ECHO, '\0' };
const   char    go_ahead_str    [] = { IAC, GA, '\0' };
int     select          args( ( int width, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout ) );
#endif

/* OS-dependent declarations. */
#if     defined(_AIX)
#include <sys/select.h>
int     accept          args( ( int s, struct sockaddr *addr, int *addrlen ) );
int     bind            args( ( int s, struct sockaddr *name, int namelen ) );
void    bzero           args( ( char *b, int length ) );
int     getpeername     args( ( int s, struct sockaddr *name, int *namelen ) );
int     getsockname     args( ( int s, struct sockaddr *name, int *namelen ) );
// int     gettimeofday    args( ( struct timeval *tp, struct timezone *tzp ) );
int     gettimeofday    args( ( struct timeval *tp, void *tzp ) );
int     listen          args( ( int s, int backlog ) );
int     setsockopt      args( ( int s, int level, int optname, void *optval,
                            int optlen ) );
int     socket          args( ( int docomm.cmain, int type, int protocol ) );
#endif

#if     defined(apollo)
#include <unistd.h>
void    bzero           args( ( char *b, int length ) );
#endif

#if     defined(__hpux)
int     accept          args( ( int s, void *addr, int *addrlen ) );
int     bind            args( ( int s, const void *addr, int addrlen ) );
void    bzero           args( ( char *b, int length ) );
int     getpeername     args( ( int s, void *addr, int *addrlen ) );
int     getsockname     args( ( int s, void *name, int *addrlen ) );
// int     gettimeofday    args( ( struct timeval *tp, struct timezone *tzp ) );
int     gettimeofday    args( ( struct timeval *tp, void *tzp ) );
int     listen          args( ( int s, int backlog ) );
int     setsockopt      args( ( int s, int level, int optname,
                                const void *optval, int optlen ) );
int     socket          args( ( int domain, int type, int protocol ) );
#endif

#if     defined(interactive)
#include <net/errno.h>
#include <sys/fnctl.h>
#endif

#if     defined(linux)
/*  Linux shouldn't need these. If you have a problem compiling, try
    uncommenting these functions. */
/*
int     accept          args( ( int s, struct sockaddr *addr, int *addrlen ) );
int     bind            args( ( int s, struct sockaddr *name, int namelen ) );
int     getpeername     args( ( int s, struct sockaddr *name, int *namelen ) );
int     getsockname     args( ( int s, struct sockaddr *name, int *namelen ) );
int     listen          args( ( int s, int backlog ) );
*/

int     close           args( ( int fd ) );
// int     gettimeofday    args( ( struct timeval *tp, struct timezone *tzp ) );
int     gettimeofday    args( ( struct timeval *tp, void *tzp ) );
int     read            args( ( int fd, char *buf, int nbyte ) );
int     select          args( ( int width, fd_set *readfds, fd_set *writefds,
                            fd_set *exceptfds, struct timeval *timeout ) );
int     socket          args( ( int domain, int type, int protocol ) );
int     write           args( ( int fd, const char *buf, int nbyte ) );
#endif

#if     defined(MIPS_OS)
extern  int             errno;
#endif

#if     defined(NeXT)
int     close           args( ( int fd ) );
int     fcntl           args( ( int fd, int cmd, int arg ) );
#if     !defined(htons)
u_short htons           args( ( u_short hostshort ) );
#endif
#if     !defined(ntohl)
u_long  ntohl           args( ( u_long hostlong ) );
#endif
int     read            args( ( int fd, char *buf, int nbyte ) );
int     select          args( ( int width, fd_set *readfds, fd_set *writefds,
                            fd_set *exceptfds, struct timeval *timeout ) );
int     write           args( ( int fd, const char *buf, int nbyte ) );
#endif

#if     defined(sequent)
int     accept          args( ( int s, struct sockaddr *addr, int *addrlen ) );
int     bind            args( ( int s, struct sockaddr *name, int namelen ) );
int     close           args( ( int fd ) );
int     fcntl           args( ( int fd, int cmd, int arg ) );
int     getpeername     args( ( int s, struct sockaddr *name, int *namelen ) );
int     getsockname     args( ( int s, struct sockaddr *name, int *namelen ) );
int     gettimeofday    args( ( struct timeval *tp, struct timezone *tzp ) );
#if     !defined(htons)
u_short htons           args( ( u_short hostshort ) );
#endif
int     listen          args( ( int s, int backlog ) );
#if     !defined(ntohl)
u_long  ntohl           args( ( u_long hostlong ) );
#endif
int     read            args( ( int fd, char *buf, int nbyte ) );
int     select          args( ( int width, fd_set *readfds, fd_set *writefds,
                            fd_set *exceptfds, struct timeval *timeout ) );
int     setsockopt      args( ( int s, int level, int optname, caddr_t optval,
                            int optlen ) );
int     socket          args( ( int domain, int type, int protocol ) );
int     write           args( ( int fd, const char *buf, int nbyte ) );
#endif

/* This includes Solaris Sys V as well */
#if defined(sun)
int     accept          args( ( int s, struct sockaddr *addr, int *addrlen ) );
int     bind            args( ( int s, struct sockaddr *name, int namelen ) );
void    bzero           args( ( char *b, int length ) );
int     close           args( ( int fd ) );
int     getpeername     args( ( int s, struct sockaddr *name, int *namelen ) );
int     getsockname     args( ( int s, struct sockaddr *name, int *namelen ) );
int     listen          args( ( int s, int backlog ) );
int     read            args( ( int fd, char *buf, int nbyte ) );
int     select          args( ( int width, fd_set *readfds, fd_set *writefds,
                            fd_set *exceptfds, struct timeval *timeout ) );

#if !defined(__SVR4)
int     gettimeofday    args( ( struct timeval *tp, struct timezone *tzp ) );

#if defined(SYSV)
int setsockopt          args( ( int s, int level, int optname,
                            const char *optval, int optlen ) );
#else
int     setsockopt      args( ( int s, int level, int optname, void *optval,
                            int optlen ) );
#endif
#endif
int     socket          args( ( int domain, int type, int protocol ) );
int     write           args( ( int fd, const char *buf, int nbyte ) );
#endif

#if defined(ultrix)
int     accept          args( ( int s, struct sockaddr *addr, int *addrlen ) );
int     bind            args( ( int s, struct sockaddr *name, int namelen ) );
void    bzero           args( ( char *b, int length ) );
int     close           args( ( int fd ) );
int     getpeername     args( ( int s, struct sockaddr *name, int *namelen ) );
int     getsockname     args( ( int s, struct sockaddr *name, int *namelen ) );
int     gettimeofday    args( ( struct timeval *tp, struct timezone *tzp ) );
int     listen          args( ( int s, int backlog ) );
int     read            args( ( int fd, char *buf, int nbyte ) );
int     select          args( ( int width, fd_set *readfds, fd_set *writefds,
                            fd_set *exceptfds, struct timeval *timeout ) );
int     setsockopt      args( ( int s, int level, int optname, void *optval,
                            int optlen ) );
int     socket          args( ( int domain, int type, int protocol ) );
int     write           args( ( int fd, const char *buf, int nbyte ) );
#endif

// Global variables
DESCRIPTOR_DATA * descriptor_list; // All open descriptors
CLAN_DATA       * clan_list;       // Clan list
DESCRIPTOR_DATA * d_next;          // Next descriptor in loop
DESCRIPTOR_DATA * d;               // My DESC
FILE *            fpReserve;       // Reserved file handle
FILE *            logReserve;      // Reserved for log file handle
bool              god;             // All new chars are gods!
bool              merc_down;       // Shutdown
char              str_boot_time[MAX_INPUT_LENGTH];
time_t            current_time;    // time of this pulse

int reboot_reason = 0;

/* saboteur and new */
bool personal_ban(const char *addr, const char *hosts);
char convert_in(int codepage, unsigned char source);
long EncodeTRANSLITERATION(const char *pWindows,char *pTransliteration);
long EncodeKOI8R(const char *pWindows, char *pKoi8r);
long EncodeDOS(const char *pWindows, char *pDos);
void TelnetCopy(char *dest, const char *source);
bool MOBtrigger = TRUE;  /* act() switch                 */
void write_to_buffer_old( DESCRIPTOR_DATA *d, const char *txt, int length );

int     close           args( ( int fd ) );
int     read            args( ( int fd, char *buf, int nbyte ) );
int     write           args( ( int fd, const char *buf, int nbyte ) );

/* OS-dependent local functions. */
#if defined(unix) || defined(WIN32)
void    game_loop_unix          args( ( int control ) );
int     init_socket             args( ( u_short port ) );
void    init_descriptor         args( ( int control ) );
bool    read_from_descriptor    args( ( DESCRIPTOR_DATA *d ) );
bool    write_to_descriptor     args( ( int desc, const char *txt, int length ) );
#endif

/* Other local functions (OS-independent). */
bool    check_reconnect         args( ( DESCRIPTOR_DATA *d, const char *name ) );
bool    check_playing           args( ( DESCRIPTOR_DATA *d, const char *name ) );
int     main                    args( ( int argc, char **argv ) );
void    nanny                   args( ( DESCRIPTOR_DATA *d, const char *argument ) );
bool    process_output          args( ( DESCRIPTOR_DATA *d, bool fPrompt ) );
void    read_from_buffer        args( ( DESCRIPTOR_DATA *d ) );
void    stop_idling             args( ( CHAR_DATA *ch ) );

#ifdef WITH_RPC
#include "rpc.h"
#endif /* WITH_RPC */

void segmentation_handler(int sig) { // Crash handler, generate backtrace to stderr
  void *array[10];
  size_t size;

  size = backtrace(array, 10);

  do_fprintf(stderr, "CRITICAL Error: received signal: %d\n", sig );
  backtrace_symbols_fd(array, size, 2); // 2 is stderr
  exit(1);
}

int main( int argc, char **argv )
{
  struct timeval now_time;
  u_short port;

#if defined(unix) || defined( WIN32 )
  int control;
#endif

  /* Memory debugging if needed. */
#if defined(MALLOC_DEBUG)
  malloc_debug(2);
#endif

  signal(SIGSEGV, segmentation_handler);

  PULSE_PER_SECOND=4;
  PULSE_VIOLENCE = ( 3 * PULSE_PER_SECOND);
  PULSE_MOBILE = ( 4 * PULSE_PER_SECOND);
  PULSE_MUSIC = ( 6 * PULSE_PER_SECOND);
  PULSE_TICK = (60 * PULSE_PER_SECOND);
  PULSE_UPDCHAR = (15 * PULSE_PER_SECOND);
  PULSE_AREA = (120 * PULSE_PER_SECOND);

  // Init time.
  gettimeofday( &now_time, NULL );
  current_time        = (time_t) now_time.tv_sec;
  strcpy( str_boot_time, ctime( &current_time ) );

  { // Reserve two streams for log and write to file
    FILE *fp;

    if (( fp = fopen( NULL_FILE, "a" ) ) == NULL )
    {
      perror( NULL_FILE );
      exit (1);
    }
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );

    if (( fp = fopen( LOGR_FILE, "a" ) ) == NULL )
    {
      perror( LOGR_FILE );
      exit (1);
    }
    fclose( fp );
    logReserve = fopen( LOGR_FILE, "r" );
  }

  // Get the port number
  port = 4000;
  if ( argc > 1 )
  {
    if ( !is_number( argv[1] ) )
    {
      do_fprintf( stderr, "Usage: %s [port #]\n", argv[0] );
      exit( 1 );
    }
    else if ( ( port = atoi( argv[1] ) ) <= 1024 )
    {
      do_fprintf( stderr, "Port number must be above 1024.\n" );
      exit( 1 );
    }
  }

  /* Run the game. */
#if defined(unix) || defined( WIN32 )
  control = init_socket( port );
  boot_db( );
//  if( IS_SET(global_cfg,CFG_GTFIX) ) REM_BIT(global_cfg, CFG_GTFIX);
  log_printf("ROM is ready to rock on port %d.", port );
  game_loop_unix( control );
#if !defined( WIN32 )
  close( control );
#else
  closesocket( control );
  WSACleanup();
#endif
#endif

  /* That's all, folks. */
  log_string( "Normal termination of game." );

  exit( reboot_reason );
  return reboot_reason;
}

#if defined( unix ) || defined( WIN32 )
int init_socket( u_short port )
{
  static struct sockaddr_in sa_zero;
  struct sockaddr_in sa;
  int x = 1;
  int fd;

#if !defined( WIN32 )
  if ( ( fd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
  {
    perror( "Init_socket: socket" );
    exit( 1 );
  }
#else
  WORD    wVersionRequested = MAKEWORD( 1, 1 );
  WSADATA wsaData;
  int err = WSAStartup( wVersionRequested, &wsaData );
  if ( err != 0 )
  {
    perror( "No useable WINSOCK.DLL" );
    exit( 1 );
  }

  if ( ( fd = socket( PF_INET, SOCK_STREAM, 0 ) ) < 0 )
  {
    perror( "Init_socket: socket" );
    exit( 1 );
  }
#endif
  if ( setsockopt( fd, SOL_SOCKET, SO_REUSEADDR,(char *) &x, sizeof( x ) ) < 0 )
  {
    perror( "Init_socket: SO_REUSEADDR" );
#if !defined( WIN32 )
    close( fd );
#else
    closesocket( fd );
#endif
    exit( 1 );
  }

#if defined( SO_DONTLINGER ) && !defined( SYSV )
  {
    struct  linger  ld;

    ld.l_onoff  = 1;
    ld.l_linger = 1000;

    if ( setsockopt( fd, SOL_SOCKET, SO_DONTLINGER,(char *) &ld, sizeof( ld ) ) < 0 )
    {
      perror( "Init_socket: SO_DONTLINGER" );
#if !defined( WIN32 )
      close( fd );
#else
      closesocket( fd );
#endif
      exit( 1 );
    }
  }
#endif
  sa              = sa_zero;
#if !defined( WIN32 )
  sa.sin_family   = AF_INET;
#else
  sa.sin_family   = PF_INET;
#endif
  sa.sin_port     = htons( port );

  if ( bind( fd, (struct sockaddr *) &sa, sizeof( sa ) ) < 0 )
  {
    perror( "Init_socket: bind" );
#if !defined( WIN32 )
    close( fd );
#else
    closesocket( fd );
#endif
    exit( 1 );
  }

  if ( listen( fd, 3 ) < 0 )
  {
    perror( "Init_socket: listen" );
#if !defined( WIN32 )
    close( fd );
#else
    closesocket( fd );
#endif
    exit( 1 );
  }
  return fd;
}
#endif

#if defined(unix) || defined (WIN32)

#ifdef WITH_ANTICRASH

#include <setjmp.h>

static jmp_buf crash;

void death_handler(int sig) {
          log_printf("Received signal %d", sig);
          longjmp(crash, sig);
}

void death_hook() {
        if(IS_SET(global_cfg, CFG_BUGTRACE))
                signal(SIGINT, SIG_IGN);
        else
                signal(SIGINT, SIG_DFL);

        if(IS_SET(global_cfg, CFG_ANTICRASH)) {
                signal(SIGILL, death_handler);
                signal(SIGABRT, death_handler);
                signal(SIGFPE, death_handler);
                signal(SIGBUS, death_handler);
                signal(SIGSEGV, death_handler);
        } else {
                signal(SIGILL, SIG_DFL);
                signal(SIGABRT, SIG_DFL);
                signal(SIGFPE, SIG_DFL);
                signal(SIGBUS, SIG_DFL);
                signal(SIGSEGV, SIG_DFL);
        }
}

char *gdb_ipc(char *arg) {
        static char rc[MAX_STRING_LENGTH] = "";

        return rc;
}

int ignore_ignorecrash = 0;
int doublecrash = 0;
#endif /* WITH_ANTICRASH */

void game_loop_unix( int control )
{
  static struct timeval null_time;
  struct timeval last_time;

#if !defined( WIN32 )
  signal( SIGPIPE, SIG_IGN );
#endif
  gettimeofday( &last_time, NULL );
  current_time = (time_t) last_time.tv_sec;

#ifdef WITH_ANTICRASH
  death_hook();

  if(setjmp(crash))
  {
    if(doublecrash)
    {
       log_printf("!PANIC! Crash while reboot!");
       exit(0x12);
    }

    log_printf("!!!!!CrAsH!!!!! !!!!!CrAsH!!!!! !!!!!CrAsH!!!!! ");
    log_printf("Resuming, but game is unstable!");

    for (d=descriptor_list;d;d=d->next)
    {
      if ( !d->character || d->connected != CON_PLAYING ) continue;
      ptc(d->character, "{YХассан {RОРЕТ НА ВЕСЬ МИР: '{W!!!{R{+КРАШ{_{W!!! СПАСАЙСЯ КТО МОЖЕТ!!!{R{*'{x\n\r");
    }

    if(ignore_ignorecrash || !IS_SET(global_cfg, CFG_IGNORECRASH))
    {
      doublecrash = 1;
      reboot_reason = 0x10;
      do_reboot(NULL, "now");
    }
    else if(rebootcount==0 || rebootcount > 10)
    {
      reboot_reason = 0x11;
      do_reboot(NULL, "10");
    }

    ignore_ignorecrash = 1;
  }

#endif

  /* Main loop */
  while ( !merc_down )
  {
    fd_set in_set;
    fd_set out_set;
    fd_set exc_set;
    DESCRIPTOR_DATA *d;
    int maxdesc;

#if defined(MALLOC_DEBUG)
    if ( malloc_verify( ) != 1 ) abort( );
#endif

     /* Poll all active descriptors. */
     FD_ZERO( &in_set  );
     FD_ZERO( &out_set );
     FD_ZERO( &exc_set );
     FD_SET( control, &in_set );
     maxdesc = control;
     for ( d = descriptor_list; d; d = d->next )
     {
       maxdesc = UMAX( maxdesc, d->descriptor );
       FD_SET( d->descriptor, &in_set  );
       FD_SET( d->descriptor, &out_set );
       FD_SET( d->descriptor, &exc_set );
     }

     if ( select( maxdesc+1, &in_set, &out_set, &exc_set, &null_time ) < 0 )
     {
       perror( "Game_loop: select: poll" );
       exit( 1 );
     }
     // New connection?
     if ( FD_ISSET( control, &in_set ) ) init_descriptor( control );

     // Kick out the freaky folks.
     for ( d = descriptor_list; d != NULL; d = d_next )
     {
       d_next = d->next;
       if ( FD_ISSET( d->descriptor, &exc_set ) )
       {
         FD_CLR( (unsigned)d->descriptor, &in_set );
         FD_CLR( (unsigned)d->descriptor, &out_set );
         if ( d->character && d->connected == CON_PLAYING)
           save_char_obj( d->character );
         d->outtop = 0;
         close_socket( d );
       }
     }

     // Process input.
     for ( d = descriptor_list; d != NULL; d = d_next )
     {
       d_next      = d->next;
       d->fcommand = FALSE;

       if ( FD_ISSET( d->descriptor, &in_set ) )
       {
         if ( d->character ) d->character->timer = 0;
         if ( !read_from_descriptor( d ) )
         {
           FD_CLR( (unsigned)d->descriptor,&out_set );
           if ( d->character && d->connected == CON_PLAYING)
             save_char_obj( d->character );
           d->outtop   = 0;
           close_socket( d );
           continue;
         }
       }

       if (d->character && d->character->daze > 0) --d->character->daze;
       if ( d->character && d->character->wait > 0 )
       {
         --d->character->wait;
         continue;
       }

        read_from_buffer( d );
        if (d->character && d->connected==CON_PLAYING
          && !EMPTY(d->character->runbuf))
        {
          run(d->character);
          continue;
        }

        if ( d->incomm[0] != '\0' )
        {
          d->fcommand     = TRUE;
          stop_idling( d->character );

          if ( d->showstr_point ) show_string( d, d->incomm );
          else if ( d->pString ) string_add( d->character, d->incomm );
          else switch ( d->connected )
          {
            case CON_PLAYING:
             if ( !run_olc_editor( d ) ) substitute_alias( d, d->incomm );
             break;
            default:
             nanny( d, d->incomm );
             break;
          }
          d->incomm[0]    = '\0';
        }
     }

     /* Autonomous game motion. */
    update_handler( );

    /* Output.*/
    for ( d = descriptor_list; d != NULL; d = d_next )
    {
      d_next = d->next;

      if ( ( d->fcommand || d->outtop > 0 )
        &&   FD_ISSET(d->descriptor, &out_set) )
      {
        if ( !process_output( d, TRUE ) )
        {
          if ( d->character != NULL && d->connected == CON_PLAYING)
            save_char_obj( d->character );
          d->outtop   = 0;
          close_socket( d );
        }
      }
    }

    /* Synchronize to a clock.
     * Sleep( last_time + 1/PULSE_PER_SECOND - now ).
     * Careful here of signed versus unsigned arithmetic.*/
#ifdef WITH_RPC
    while(1) {
      struct timeval now_time;
      long secDelta;
      long usecDelta;

      gettimeofday( &now_time, NULL );
      usecDelta   = ((int) last_time.tv_usec) - ((int) now_time.tv_usec)
                        + 1000000 / PULSE_PER_SECOND;
      secDelta    = ((int) last_time.tv_sec ) - ((int) now_time.tv_sec );
      while ( usecDelta < 0 )
      {
        usecDelta += 1000000;
        secDelta  -= 1;
      }

      while ( usecDelta >= 1000000 )
      {
        usecDelta -= 1000000;
        secDelta  += 1;
      }

      if ( secDelta > 0 || ( secDelta == 0 && usecDelta > 0 ) )
      {
        int rc;
        fd_set fds = svc_fdset;
        struct timeval stall_time;

        stall_time.tv_usec = usecDelta;
        stall_time.tv_sec  = secDelta;

        if ( (rc=select(FD_SETSIZE, &fds, NULL, NULL, &stall_time )) < 0 ) {
                perror( "Game_loop: select: stall" );
                exit( 1 );
        } else if(rc) svc_getreqset(&fds);
      } else break;
    }
#elif !defined(WIN32)
    {
      struct timeval now_time;
      long secDelta;
      long usecDelta;

      gettimeofday( &now_time, NULL );
      usecDelta   = ((int) last_time.tv_usec) - ((int) now_time.tv_usec)
                        + 1000000 / PULSE_PER_SECOND;
      secDelta    = ((int) last_time.tv_sec ) - ((int) now_time.tv_sec );
      while ( usecDelta < 0 )
      {
        usecDelta += 1000000;
        secDelta  -= 1;
      }

      while ( usecDelta >= 1000000 )
      {
        usecDelta -= 1000000;
        secDelta  += 1;
      }

      if ( secDelta > 0 || ( secDelta == 0 && usecDelta > 0 ) )
      {
        struct timeval stall_time;

        stall_time.tv_usec = usecDelta;
        stall_time.tv_sec  = secDelta;
        if ( select( 0, NULL, NULL, NULL, &stall_time ) < 0 )
        {
          perror( "Game_loop: select: stall" );
          exit( 1 );
        }
      }
    }
#else
    {
      int times_up;
      int nappy_time;
      struct _timeb start_time;
      struct _timeb end_time;
      _ftime( &start_time );
      times_up = 0;

      while( times_up == 0 )
      {
        _ftime( &end_time );
        if ( ( nappy_time = (int) ( 1000 *
                            (double) ( ( end_time.time - start_time.time ) +
                            ( (double) ( end_time.millitm -
                            start_time.millitm ) / 1000.0 ) ) ) ) >=
                            (double)( 1000 / PULSE_PER_SECOND ) )
                            times_up = 1;
        else
        {
          Sleep( (int) ( (double) ( 1000 / PULSE_PER_SECOND ) -
                                  (double) nappy_time ) );
          times_up = 1;
        }
      }
    }
#endif

    gettimeofday( &last_time, NULL );
    current_time = (time_t) last_time.tv_sec;

#ifdef WITH_ANTICRASH
    ignore_ignorecrash = 0;
#endif
  }
}
#endif

#if defined(unix) || defined( WIN32 )
void init_descriptor( int control )
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *dnew=NULL;
  struct sockaddr_in sock;
  struct hostent *from;
  int desc;
  unsigned int size;

  size = sizeof(sock);
  getsockname( control, (struct sockaddr *) &sock, &size );
  if ( ( desc = accept( control, (struct sockaddr *) &sock, &size) ) < 0 )
  {
    perror( "New_descriptor: accept" );
    return;
  }

#if !defined(FNDELAY)
#define FNDELAY O_NDELAY
#endif

#if !defined( WIN32 )
  if ( fcntl( desc, F_SETFL, FNDELAY ) == -1 )
  {
    perror( "New_descriptor: fcntl: FNDELAY" );
    return;
  }
#endif

  // Cons a new descriptor.
  dnew = new_descriptor();
  dnew->descriptor    = desc;
  if (autologin_code!=0)
  {
    dnew->codepage=autologin_code;
    dnew->connected=CON_GET_NAME;
  }
  else dnew->connected = CON_GET_CODEPAGE;
  dnew->showstr_head  = NULL;
  dnew->showstr_point = NULL;
  dnew->outsize       = 2000;
  dnew->pEdit         = NULL; // OLC
  dnew->pString       = NULL; // OLC
  dnew->editor        = 0;    // OLC
  dnew->outbuf        = alloc_mem( dnew->outsize );

  size = sizeof(sock);
  if ( getpeername( desc, (struct sockaddr *) &sock, &size ) < 0 )
  {
    perror( "New_descriptor: getpeername" );
    dnew->host = str_dup( "(unknown)" );
  }
  else
  {
    // Would be nice to use inet_ntoa here but it takes a struct arg,
    // which ain't very compatible between gcc and system libraries.
    int addr;

    addr = ntohl( sock.sin_addr.s_addr );
    do_printf( buf, "%3d.%3d.%3d.%3d ",
      ( addr >> 24 ) & 0xFF, ( addr >> 16 ) & 0xFF,
      ( addr >>  8 ) & 0xFF, ( addr       ) & 0xFF);
    wiznet(log_buf,NULL,NULL,WIZ_ADDR,0);
    if ( check_ban(buf,BAN_ALL))
    {
      write_to_descriptor( desc,"Host is down.\n\r", 0 );
#if !defined( WIN32 )
      close( desc );
#else
      closesocket( desc );
#endif
      free_descriptor(dnew);
      return;
    }
    from=gethostbyaddr((char *) &sock.sin_addr,sizeof(sock.sin_addr),AF_INET);
    dnew->host = str_dup( from ? from->h_name : buf );
  }

  if ( check_ban(dnew->host,BAN_ALL))
  {
    write_to_descriptor( desc,"Host is down.\n\r", 0 );
#if !defined( WIN32 )
    close( desc );
#else
    closesocket( desc );
#endif
    free_descriptor(dnew);
    return;
  }

  // Init descriptor data.
  dnew->next                  = descriptor_list;
  descriptor_list             = dnew;

  if (autologin_code==0)
  {
    extern char * help_greeting;
    if ( help_greeting[0] == '.' )
      write_to_buffer( dnew, help_greeting+1, 0 );
    else
      write_to_buffer( dnew, help_greeting  , 0 );
  }
  else
  {
    write_to_buffer( dnew, "\n\rНазови свое имя, странник:  ", 0 );
  }
}
#endif

void close_socket( DESCRIPTOR_DATA *dclose )
{
  CHAR_DATA *ch;
  DESCRIPTOR_DATA *d;

  if (dclose->outtop > 0) process_output( dclose, FALSE );

  if (!dclose || !IS_VALID(dclose))
  {
    log_printf("BUG - invalid descriptor in close_socket");
    return;
  }

  for (d=descriptor_list;d;d=d->next)
    if (d->snoop_by==dclose) d->snoop_by=NULL;

  if ( ( ch = dclose->character ) != NULL )
  {
    log_printf( "Closing link to %s.", ch->name );

    // cut down on wiznet spam when rebooting
    if ( dclose->connected == CON_PLAYING && !merc_down)
    {
      act( "$c1 потерял$r связь.", ch, NULL, NULL, TO_ROOM );
      wiznet("$C1 gets Lostlink.",ch,NULL,WIZ_LINKS,0);
      ch->desc = NULL;
    }
    else
    {
      if ( ch == char_list ) char_list = ch->next;
      else
      {
        CHAR_DATA *prev;
        for ( prev = char_list; prev; prev = prev->next )
        {
          if ( prev->next == ch)
          {
            prev->next = ch->next;
            break;
          }
        }
        if (!prev) bug( "Close_socket: dclose->character in char_list not found.", 0 );
      }
      dclose->character=NULL;
      ch->desc=NULL;
      free_char(ch);
    }
  }
  if ( d_next == dclose ) d_next = d_next->next;

#if !defined( WIN32 )
  close( dclose->descriptor );
#else
  closesocket( dclose->descriptor );
#endif
  free_descriptor(dclose);
}

bool read_from_descriptor( DESCRIPTOR_DATA *d )
{
  int iStart;

  // Hold horses if pending command already.
  if ( d->incomm[0] != '\0' ) return TRUE;

  // Check for overflow.
  iStart = strlen(d->inbuf);
  if ( iStart >= sizeof(d->inbuf) - 10 )
  {
    log_printf( "%s input overflow!", d->host );
    write_to_descriptor( d->descriptor, "\n\r* PUT A LID ON IT! *\n\r", 0 );
    return FALSE;
  }

  // Snarf input.
  for ( ; ; )
  {
    int nRead;

#if !defined( WIN32 )
    nRead = read( d->descriptor, d->inbuf + iStart,
      sizeof( d->inbuf ) - 10 - iStart );
#else
    nRead = recv( d->descriptor, d->inbuf + iStart,
      sizeof( d->inbuf ) - 10 - iStart, 0 );
#endif

    if ( nRead > 0 )
    {
      int cnt;

      for (cnt=iStart; cnt<=iStart+nRead; cnt++)
        if (d->inbuf[cnt] == 27) d->inbuf[cnt]=' ';

      iStart += nRead;
      if ( d->inbuf[iStart-1] == '\n' || d->inbuf[iStart-1] == '\r' )
        break;
    }
    else if ( nRead == 0 )
    {
      log_string( "BUG:EOF encountered on read." );
      return FALSE;
    }
#if !defined( WIN32 )
    else if ( errno == EWOULDBLOCK || errno == EAGAIN ) break;
#endif
#if defined( WIN32 )
    else if ( WSAGetLastError() == WSAEWOULDBLOCK || errno == EAGAIN ) break;
#endif
    else
    {
      perror( "Read_from_descriptor" );
      return FALSE;
    }
  }
  d->inbuf[iStart] = '\0';
  return TRUE;
}

// Transfer one line from input buffer to input line.
void read_from_buffer( DESCRIPTOR_DATA *d )
{
  int i, j, k;

  /* Hold horses if pending command already. */
  if ( d->incomm[0] != '\0' ) return;

  /* Look for at least one new line. */
  for ( i = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
  {
    if ( d->inbuf[i] == '\0' ) return;
  }

  /* Canonical input processing. */
  for ( i = 0, k = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
  {
    if ( k >= MAX_INPUT_LENGTH - 12 )
    {
      write_to_descriptor( d->descriptor, "Line too long.\n\r", 0 );

      /* skip the rest of the line */
      for ( ; d->inbuf[i] != '\0'; i++ )
      {
        if ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' ) break;
      }
      d->inbuf[i]   = '\n';
      d->inbuf[i+1] = '\0';
      break;
    }
    if ( d->inbuf[i] == '\b' && k > 0 ) --k;
    else d->incomm[k++]=convert_in(d->codepage,(unsigned char)d->inbuf[i]); /* Decoding */
  }

  // Finish off the line
  if ( k == 0 ) d->incomm[k++] = ' ';
  d->incomm[k] = '\0';

  // Begin anti-spam system
  if ( k > 1 || d->incomm[0] == '!' )
  {
    if ( d->incomm[0] != '!' && strcmp( d->incomm, d->inlast ) )
    {
      d->repeat = 0;
    }
    else
    {

      if (++d->repeat > 3 && d->character && d->connected == CON_PLAYING)
      {
        char command[MAX_INPUT_LENGTH+2];
        int cmd;
        struct cmd_type *cmd_ptr = NULL;
        bool found=FALSE;
        bool social=FALSE;

        one_argument( d->inlast, command );
        for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
        {
          if ( command[0] == cmd_table[cmd].name[0]
            && ((!IS_SET(cmd_table[cmd].flag, FULL)
            && !str_prefix( command, cmd_table[cmd].name))
            || ( IS_SET(cmd_table[cmd].flag, FULL)
            && !str_cmp( command, cmd_table[cmd].name))))
          {
            cmd_ptr = &cmd_table[cmd];
            found = TRUE;
            break;
          }
        }
        if (!found)
        {
          for ( cmd = 0; social_table[cmd].name[0] != '\0'; cmd++ )
          {
            if ( command[0] == social_table[cmd].name[0]
              && !str_prefix( command, social_table[cmd].name ) )
            {
              found = TRUE;
              break;
            }
          }
          if (found) social=TRUE;
        }
        if (found && ( (social && IS_SET(social_table[cmd].flag,SOC_NOSPAM)) ||
         (!social && IS_SET(cmd_ptr->flag,SPAM)) ))
        {
          if (d->repeat>4)
          {
            WAIT_STATE(d->character,PULSE_VIOLENCE*12);
            log_printf("%s from %s input spamming!", d->character->name,d->host );
            stc("{RPlz, NO SPAM more!{x\n\r",d->character);

            wiznet("Spamming from char $C1 !",
              d->character,NULL,WIZ_SPAM,get_trust(d->character));

            if (d->incomm[0] == '!')
              wiznet(d->inlast,d->character,NULL,WIZ_SPAM,get_trust(d->character));
            else wiznet(d->incomm,d->character,NULL,WIZ_SPAM,get_trust(d->character));
            d->repeat = 0;
          }
          else stc("{RПредупреждение: обнаружен SPAM!{x\n\r",d->character);
        }
      }
    }
  }
  // end of anti-spam system

  // Do '!' substitution
  if ( d->incomm[0] == '!' ) strcpy( d->incomm, d->inlast );
  else strcpy( d->inlast, d->incomm );

  // Shift the input buffer
  while ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' ) i++;
  for ( j = 0; ( d->inbuf[j] = d->inbuf[i+j] ) != '\0'; j++ ) ;
  return;
}

// -----------------------------------------------------------------------
// Process output buffer of the characters as well as printing the prompt
// and battle data
// -----------------------------------------------------------------------
bool process_output (DESCRIPTOR_DATA * d, bool fPrompt)
{
  extern bool merc_down ;

  // bust a prompt.
  if (!merc_down)
  {
    if (d->showstr_point)
        write_to_buffer (d, "[ -- Нажми ENTER -- ]\n\r", 0) ; else
    if (fPrompt && d->pString && d->connected == CON_PLAYING)
      write_to_buffer (d, "> ", 2) ; else
    if (fPrompt && d->connected == CON_PLAYING && d->character)
    {
      CHAR_DATA * ch     ;
      CHAR_DATA * victim ;

      ch = d->character ;

      // battle prompt
      if ((victim = ch->fighting) != NULL && can_see (ch, victim, CHECK_LVL))
      {
        int  percent ;
        char * pbuff ;
        char buf    [MAX_STRING_LENGTH]   ;
        char buffer [MAX_STRING_LENGTH*2] ;
        char wound  [100] ;

        // calculate percentage of the victims hp
        if (victim->max_hit > 0) percent = victim->hit * 100 / victim->max_hit ;
        else percent = -1 ;

        // print this data for everyone
        if (percent >= 100) do_printf (wound, "{cв пpекpасном состоянии.{g(%d%%){x",                          percent) ; else
        if (percent >= 90)  do_printf (wound, "{bимеет несколько цаpапин.{g(%d%%){x",                         percent) ; else
        if (percent >= 75)  do_printf (wound, "{yимеет несколько маленьких pан и синяков.{g(%d%%){x",         percent) ; else
        if (percent >= 50)  do_printf (wound, "{gимеет довольно много pан.{g(%d%%){x",                        percent) ; else
        if (percent >= 30)  do_printf (wound, "{gимеет несколько больших опасных pан и {mцаpапин.{g(%d%%){x", percent) ; else
        if (percent >= 15)  do_printf (wound, "{mвыглядит сильно повpежденным.{g(%d%%){x",                    percent) ; else
        if (percent >= 0)   do_printf (wound, "{rв ужасном состоянии.{g(%d%%){x",                             percent) ; else
                            do_printf (wound, "{rвыглядит..хмм..немного убитым...{g(%d%%){x",                 percent) ;

        do_printf (buf, "%s %s \n\r",
                   IS_NPC (victim) ? get_char_desc (victim, '1') :
                   victim->name, wound) ;

        buf[0] = UPPER (buf[0]) ;
        pbuff  = buffer ;

        colourconv      (pbuff, buf, d->character) ;
        write_to_buffer (d, buffer, 0) ;
      }

      ch = d->character ;

      // prompt
      if (IS_SET (ch->comm, COMM_PROMPT))
      {
        write_to_buffer (d,"\n\r", 2) ;
        bust_a_prompt   (d->character) ;
      }

      if (IS_SET(ch->comm, COMM_TELNET_GA)) {
        write_to_buffer (d, go_ahead_str, 0); // write_to_buffer (d, "\xff\xff\0", 0);

      }
    }
  }

  // short-circuit if nothing to write.
  if (d->outtop == 0) return TRUE ;

  // snoop-o-rama
  if (d->snoop_by != NULL)
  {
    if (d->character != NULL)
      write_to_buffer (d->snoop_by, d->character->name, 0) ;

    write_to_buffer (d->snoop_by, "> ", 2) ;
    write_to_buffer (d->snoop_by, d->outbuf, d->outtop) ;
  }

  // OS-dependent output
  if (!write_to_descriptor (d->descriptor, d->outbuf, d->outtop))
  {
    d->outtop = 0 ;
    return FALSE ;
  }
  else
  {
    d->outtop = 0 ;
    return TRUE ;
  }
}

// -----------------------------------------------------------------------
// Bust a prompt (player settable prompt)
// -----------------------------------------------------------------------
void bust_a_prompt (CHAR_DATA * ch)
{
  char buf    [MAX_STRING_LENGTH] ;
  char buf2   [MAX_STRING_LENGTH] ;
  char buffer [MAX_STRING_LENGTH * 2] ;
  char doors  [MAX_INPUT_LENGTH] ;

  const char * str ;
  const char * i ;
  char * point ;
  char * pbuff ;
  EXIT_DATA * pexit ;
  bool found ;

  const char * dir_name[] = {"{GN{x","{GE{x","{GS{x","{GW{x","{GU{x","{GD{x", "{Rn{x", "{Re{x", "{Rs{x", "{Rw{x", "{Ru{x", "{Rd{x"} ;
  int door ;

  point = buf ;
  str   = ch->prompt ;

  // on AFK status just AFK is printed
  if (IS_SET (ch->comm, COMM_AFK))
  {
    stc ("<AFK> ", ch) ;
    return ;
  }

  // show default prompt
  if (!str || str[0] == '\0')
  {
    ptc (ch, "{c<%dhp %dm %dmv>{x %s",
         ch->hit, ch->mana, ch->move, ch->prefix) ;
    return ;
  }

  // parse prompt
  while (*str != '\0')
  {
    // util next prompt variables if found
    if (*str != '%')
    {
      *point++ = *str++ ;
      continue ;
    }
    ++str ;

    switch (*str)
    {
    default:  // skip over unknown variables
      i = " " ; break ;

    case 'e': // show exits

      found    = FALSE ;
      doors[0] = '\0' ;

      // see what exits are in this room
      for (door = 0 ; door < 6 ; door++)
      {
        if ((pexit = ch->in_room->exit[door]) != NULL &&
             pexit->u1.to_room != NULL                &&
            (can_see_room (ch, pexit->u1.to_room)     ||
             (IS_AFFECTED(ch, AFF_INFRARED)           &&
              !IS_AFFECTED(ch,AFF_BLIND))))
        {
          // there is an exit and we can see it
          found = TRUE ;
          strcat (doors,
                  IS_SET(pexit->exit_info, EX_CLOSED) ? dir_name[door + 6] :
                                                        dir_name[door]) ;
        }
      }

      // no exits
      if (!found) strcat (buf, "none") ;

      do_printf (buf2, "%s", doors) ;
      i = buf2 ; break ;

    case 'c': // new line

      do_printf (buf2, "%s", "\n\r") ;
      i = buf2 ; break ;

    case 'C': // show criminal

      do_printf (buf2, "%s%d", (IS_SET (ch->act, PLR_WANTED)) ? "W" : "",
                 ch->criminal) ;
      i = buf2 ; break ;

    case 'h': // current hp count

      do_printf (buf2, "%d", ch->hit) ;
      i = buf2 ; break ;

    case 'H': // max hp count

      do_printf (buf2, "%d", ch->max_hit) ;
      i = buf2 ; break ;

    case 'm': // current mana count

      do_printf (buf2, "%d", ch->mana) ;
      i = buf2 ; break ;

    case 'M': // max mana count

      do_printf (buf2, "%d", ch->max_mana) ;
      i = buf2 ; break ;

    case 'v': // current moves count

      do_printf (buf2, "%d", ch->move) ;
      i = buf2 ; break ;

    case 'V': // max moves count
      do_printf (buf2, "%d", ch->max_move) ;
      i = buf2 ; break ;

    case 'x': // current exp
      do_printf (buf2, "%d", ch->exp) ;
      i = buf2 ; break ;

    case 'X': // exp to the next level

      do_printf (buf2, "%d", IS_NPC(ch) ? 0 :
                 (ch->level + 1) * exp_per_level (ch, ch->pcdata->points) -
                 ch->exp) ;
      i = buf2 ; break ;

    case 'g': // gold count

      do_printf (buf2, "%u", ch->gold) ;
      i = buf2 ; break ;

    case 's': // silver count

      do_printf (buf2, "%u", ch->silver) ;
      i = buf2 ; break ;

    case 'a': // alignment

      if (ch->level > 9) do_printf (buf2, "%d", ch->alignment) ;
      else do_printf (buf2, "%s", IS_GOOD (ch) ? "good" :
                                  IS_EVIL (ch) ? "evil" : "neutral") ;
      i = buf2 ; break ;

    case 'r': // room name
      if (ch->in_room != NULL)
        do_printf (buf2, "%s",
                   ((!IS_NPC (ch) && IS_SET (ch->act, PLR_HOLYLIGHT)) ||
                    (!IS_AFFECTED  (ch, AFF_BLIND) &&
                     !room_is_dark (ch->in_room)))
                    ? ch->in_room->name : "darkness") ;
      else do_printf (buf2, " ") ;
      i = buf2 ; break ;

    case 'R': // vnum of the room (imm only)

      if (IS_IMMORTAL (ch) && ch->in_room != NULL)
           do_printf (buf2, "%u", ch->in_room->vnum) ;
      else do_printf (buf2, " ") ;
      i = buf2 ; break ;

    case 'z': // area name (imm only)

      if (IS_IMMORTAL (ch) && ch->in_room != NULL)
           do_printf (buf2, "%s", ch->in_room->area->name) ;
      else do_printf (buf2, " ") ;
      i = buf2 ; break ;

    case 'Z': // sector type (imm only)

      if (IS_IMMORTAL (ch) && ch->in_room != NULL)
           do_printf (buf2, "%s", flag_string(sector_flags, ch->in_room->sector_type)) ;
      else do_printf (buf2, " ") ;
      i = buf2 ; break ;

    case '%': // single percent sign

      do_printf (buf2, "%%") ;
      i = buf2 ; break ;

    case 'o': // name of object edited in OLC

      if (IS_IMMORTAL (ch)) do_printf (buf2, "%s", olc_ed_name(ch)) ;
      else                  do_printf (buf2, " ") ;
      i = buf2 ; break ;

    case 'O': // vnum of object edited in OLC

      if (IS_IMMORTAL (ch)) do_printf (buf2, "%s", olc_ed_vnum(ch)) ;
      else                  do_printf (buf2, " ") ;
      i = buf2 ; break ;

    case 'Q': // quest points count

      do_printf (buf2, "%d", ch->questpoints) ;
      i = buf2 ; break ;

    case 'q': // quest time count

      if (ch->countdown > 0) do_printf (buf2, "-%d", ch->countdown) ;
      else                   do_printf (buf2, "+%d", ch->nextquest) ;
      i = buf2 ; break ;

    case 'd': // time of day

      switch (weather_info.sunlight)
      {
      case SUN_LIGHT: do_printf (buf2, "Утро")  ; break ;
      case SUN_RISE : do_printf (buf2, "День")  ; break ;
      case SUN_SET:   do_printf (buf2, "Вечер") ; break ;
      case SUN_DARK:  do_printf (buf2, "Ночь")  ; break ;
      }
      i = buf2 ; break ;

    case 'T': // target's hp percentage

      if (ch->position == POS_FIGHTING && ch->fighting != NULL &&
          can_see (ch, ch->fighting, CHECK_LVL))
      {
        CHAR_DATA * victim = ch->fighting ;
        int percent = -1 ;

        if (victim->max_hit > 0)
          percent = victim->hit * 100 / victim->max_hit ;

        do_printf (buf2, "%d%%", percent) ;
      }
      else do_printf (buf2, "none") ;
      i = buf2 ; break ;

    }
    ++str ;
    while ((*point = *i) != '\0') ++point, ++i ;
  }

  *point = '\0' ;
  pbuff  = buffer ;

  colourconv      (pbuff, buf, ch) ;
  write_to_buffer (ch->desc, buffer, 0) ;

  // no prefix
  if (ch->prefix[0] != '\0') write_to_buffer (ch->desc, ch->prefix, 0) ;
}

// Append onto an output buffer.
void write_to_buffer( DESCRIPTOR_DATA *d, const char *txt, int length )
{
  int l;

  if (!d || !IS_VALID(d)) return;

  if ( length <= 0 ) length = strlen(txt); //Find length in case caller didn't.
  else if(strlen(txt) > (unsigned)length)
  {
   /* XXX
    * APAR: memmory blocks screwing.
    *       (extrem unstability after `snoop' command in some cases)
    * STAT: TEMPFIXED
    * This fix is only for testing/tracing.
    * Real location of the bug is in feeding data into outbuf in
    * EncodeXXX() routines (charset.c). They shouldn't relay on
    * NUL strings terminators.
    * (unicorn)
    */
    log_printf("[BUG] (unicorn) !%s! "
               "write_to_buffer argument length assertion failed."
               "%d > %d.",(d->character && d->character->name) ?
               d->character->name : "unknown",strlen(txt), length);
    return;
  }

  // Initial \n\r if needed.
  if ( d->outtop == 0 && !d->fcommand )
  {
    d->outbuf[0]    = '\n';
    d->outbuf[1]    = '\r';
    d->outtop       = 2;
  }

  // Expand the buffer as needed.
  while ( d->outtop + length >= d->outsize )
  {
    char *outbuf;

    if (d->outsize >= 32000)
    {
      bug("Buffer overflow. Closing.\n\r",0);
      d->outtop=0;
      close_socket(d);
      return;
    }
    outbuf = alloc_mem( 2 * d->outsize);
    strncpy( outbuf, d->outbuf, d->outtop );
    free_mem( d->outbuf, d->outsize );
    d->outbuf   = outbuf;
    d->outsize *= 2;
  }

  //  Charset begins there
  switch (d->codepage)
  {
    case 1:
      EncodeKOI8R(txt, d->outbuf + d->outtop);
      break;
    case 3:
      EncodeDOS(txt, d->outbuf + d->outtop);
      break;
    case 4:
      /* XXX FIXME WTF!
       *
       * This causes allocated memmory overflow
       * CAUTION: !THIS IS NOTFIXED BUG LEFT FOR FIGURING UP DETAILS!
       *
       * APAR:
       *  - SIGSEGV while `rgFreeList[iList] = * ((void **) rgFreeList[iList]',
       *  - Allready allocated blocks curruption.
       *
       * (unicorn)
       */
      l = EncodeTRANSLITERATION(txt, d->outbuf + d->outtop);
      if(l > d->outsize)
              bug("TRANSLIT conversion overflow could screw up memory blocks.", l);
      break;
    default:
      TelnetCopy( d->outbuf + d->outtop, txt );
      break;
  }
  d->outtop += length;
  return;
}

/* Lowest level output function.
 * Write a block of text to the file descriptor.
 * If this gives errors on very long blocks (like 'ofind all'),
 *   try lowering the max block size. */
bool write_to_descriptor( int desc, const char *txt, int length )
{
  int iStart;
  int nWrite;
  int nBlock;

  if ( length <= 0 ) length = strlen(txt);

  for ( iStart = 0; iStart < length; iStart += nWrite )
  {
    nBlock = UMIN( length - iStart, 4096 );

#if !defined( WIN32 )
    if ( ( nWrite = write( desc, txt + iStart, nBlock ) ) < 0 )
#else
    if ( ( nWrite = send( desc, txt + iStart, nBlock , 0) ) < 0 )
#endif
    { perror( "Write_to_descriptor" ); return FALSE; }
  }
  return TRUE;
}

// -----------------------------------------------------------------------
// Parse a name for acceptability
// -----------------------------------------------------------------------
bool check_parse_name (const char * name, bool new)
{
  CLAN_DATA * clan ;
  int deity;

  // "short" forms or keywords
  if (is_exact_name (name,
     "hassan all auto immortal self someone something the you loner imm mud" \
     "sab saboter sabot ")) return FALSE ;

  // "prefixes" of elders
  if( !str_prefix( name, "astel") || !str_prefix(name, "sabot") ) return FALSE;

  // closed names of deities except elders' names
  for( deity=0; deity_table[deity].name != NULL; deity++)
    if( is_exact_name( name, deity_table[deity].name)
     && !is_exact_name( name, "astellar saboteur magica adron") ) return FALSE;

  // "popular" words
  if (is_name (name,
      "fdungeon huy hyu gitler pizda mudak ssikota scikota jopa fuck sessia session" \
      "lenin urod pidar pidor gopnik debil idiot suka padla")) return FALSE ;

  // closed names for newbies
  if (new && is_exact_name (name,"illinar invader adron saboteur astellar")) return FALSE ;

  // no naming after clans
  for (clan = clan_list ; clan != NULL ; clan = clan->next)
  {
    if (LOWER(name[0]) == LOWER(clan->name[0]) &&
        !str_cmp (name, clan->name)) return FALSE ;
  }

  // short names and long are not supported
  if (strlen (name) > 12 || strlen (name) < 3) return FALSE ;

  // alphanumerics only, Lock out IllIll twits
  {
    const char * pc ;
    bool fIll, adjcaps = FALSE, cleancaps = FALSE ;
    unsigned int total_caps = 0 ;

    fIll = TRUE ;
    for (pc = name ; *pc != '\0' ; pc++ )
    {
      if (!isalpha(*pc)) return FALSE ;

      if (isupper(*pc)) // ugly anti-caps hack
      {
        if (adjcaps) cleancaps = TRUE ;
        total_caps++ ;
        adjcaps = TRUE ;
      }
      else adjcaps = FALSE ;

      if (LOWER(*pc) != 'i' && LOWER(*pc) != 'l' ) fIll = FALSE ;
    }

    if (fIll) return FALSE ;

    if (cleancaps || (total_caps > (strlen(name)) / 2 && strlen(name) < 3))
      return FALSE ;
  }

  /*
  // prevent players from naming themselves after mobs
  // temporarily (or permanently) removed by Saboteur
  {
    extern MOB_INDEX_DATA * mob_index_hash [MAX_KEY_HASH] ;
    MOB_INDEX_DATA * pMobIndex ;
    int iHash ;

    for (iHash = 0 ; iHash < MAX_KEY_HASH ; iHash++)
    {
      for (pMobIndex = mob_index_hash[iHash] ; pMobIndex != NULL ;
           pMobIndex = pMobIndex->next)
        if (is_name (name, pMobIndex->player_name)) return FALSE ;
    }
  }
  */

  return TRUE ;
}

// -----------------------------------------------------------------------
// Function to deal with sockets that have no associated player within,
// that is used for login and new character creation
// -----------------------------------------------------------------------
void nanny (DESCRIPTOR_DATA * d, const char * argument)
{
  DESCRIPTOR_DATA * d_old, * d_next ;
  CHAR_DATA       * ch ;

  char buf [MAX_STRING_LENGTH] ;
  char arg [MAX_INPUT_LENGTH]  ;
  const char * pwdnew ;
  const char * p ;
  int race, i ;

  char * temp1 ;
  time_t temp2 ;
  bool   existent ;

  // skip initial spaces
  while (isspace(*argument)) argument++ ;

  ch = d->character ;

  switch (d->connected)
  {
  default:
    bug ("Nanny: bad d->connected %d.", d->connected) ;
    close_socket( d ) ;
    return ;

  case CON_GET_CODEPAGE:

    if (EMPTY (argument))
    {
      close_socket (d) ;
      return ;
    }

    if (!isdigit (argument[0]))
    {
      write_to_buffer (d, "Invalid codepage, try again.\n\rCodepage: ", 0) ;
      return ;
    }

    d->codepage = atoi (argument) ;

    if ((d->codepage < 1) || (d->codepage > 4))
    {
      write_to_buffer (d, "Invalid codepage, try again.\n\rCodepage: ", 0) ;
      return;
    }

    write_to_buffer (d, "\n\rНазови свое имя, странник:  ", 0) ;
    d->connected = CON_GET_NAME ;
    return ;

  case CON_GET_NAME:

    if (EMPTY (argument))
    {
      close_socket (d) ;
      return;
    }

    ((char*)argument)[0] = UPPER(argument[0]);

    if (!check_parse_name (argument, FALSE))
    {
      write_to_buffer (d, "Неверное имя. Назови имя, достойное тебя.\n\rName: ", 0) ;
      return ;
    }

    // try to load character
    // now d (descriptor) is associated with a character, however that can be
    // empty character - without player file
    existent = load_char_obj (d, argument, SAVE_NORMAL);

    if( (ch = d->character ) == NULL )
    {
      close_socket(d);
      log_printf("Prohibited access for conflicting character %s from %s.\n\r", argument, d->host);
      write_to_buffer(d,"Your character is conflicting. Contact imms.\n\r", 0);
      return;
    }

    if (IS_SET (ch->act, PLR_DENY))
    {
      log_printf      ("Denying access to %s %s.", argument, d->host) ;
      write_to_buffer (d, "Доступ запрещен.\n\r", 0) ;
      close_socket    (d) ;
      return ;
    }

    if (IS_SET (global_cfg, CFG_WIZLOCK) && !IS_IMMORTAL (ch))
    {
      write_to_buffer (d, "Мир на время закрыт Богами...\n\r", 0) ;
      close_socket    (d) ;
      return ;
    }

    if (existent) // player already exists
    {
      // in autologin mode - no password is prompted
      if (autologin_pass == 1)
      {
        write_to_buffer (d, "\n\r", 2) ;

        // will reconnect already playing or disconnected character
        if (check_playing   (d, ch->name)) return ;
        if (check_reconnect (d, ch->name)) return ;

        log_printf ("%s %s has connected.", ch->name,
                    IS_SET(ch->act, PLR_AUTOSPIT) ? ch->host : d->host) ;
        wiznet     (log_buf, NULL, NULL, WIZ_SITES, get_trust (ch)) ;

        {
          char tmpbuf[128];
          do_printf(tmpbuf,"%s from %s entered.\n",ch->name,d->host);
          ID_FILE="addr.lst";
          stf(tmpbuf,NULL);
        }

        if (ch->host)
        {
          temp1 = ctime(&ch->lastlogin) ;
          temp1[strlen (temp1) - 1] = 0 ;
          log_printf("1 Char %s last connection from %s %s",ch->name,ch->host, d->host);
          ptc (ch, "\n\r{WВ последний раз этот персонаж заходил {G%s с {Y%s{x.\n\r", temp1, ch->host);
        }

        if (!IS_SET (ch->act, PLR_AUTOSPIT)) ch->host = str_dup (d->host);
        ch->lastlogin = time (&temp2) ;

        // go to the next stage without promting for password
        d->connected = CON_READ_MOTD ;
        return ;
      }

      // prompt for password
      write_to_buffer (d, "Пароль: ", 0)  ;
      ch->timer=28;
      d->connected = CON_GET_OLD_PASSWORD ;
      return ;
    }
    else // new player
    {
      if (IS_SET (global_cfg, CFG_NEWLOCK))
      {
        write_to_buffer (d, "Игра закрыта для новичков.\n\r", 0) ;
        close_socket    (d) ;
        return ;
      }

      if (check_ban (d->host, BAN_NEWBIE))
      {
        write_to_buffer (d, "С этого адреса доступ запрещен.\n\r", 0) ;
        close_socket    (d) ;
        return ;
      }

      if (!check_parse_name (argument, TRUE))
      {
        write_to_buffer (d, "Неверное имя. Назови имя, достойное тебя.\n\rName: ", 0) ;
        return ;
      }

      do_printf (buf, "%s, я правильно произношу (Y/N)? ", argument) ;
      write_to_buffer (d, buf, 0) ;

      // confirmation of this name
      d->connected = CON_CONFIRM_NEW_NAME ;
      return ;
    }
    break ;

  // get player's password
  case CON_GET_OLD_PASSWORD:

    write_to_buffer (d, "\n\r", 2) ;

    if (get_trust (ch) > 101 && check_ban (d->host, BAN_IMM))
    {
      log_printf      ("Imm Banned %s from %s", ch->name, d->host) ;
      write_to_buffer (d, "Неверный пароль.\n\r", 0) ;
      close_socket    (d) ;
      return ;
    }

    if (personal_ban (d->host, ch->pcdata->deny_addr))
    {
      log_printf      ("Personal Ban %s from %s", ch->name, d->host) ;
      write_to_buffer (d, "Неверный пароль.\n\r", 0) ;
      close_socket    (d) ;
      return ;
    }

    // check for password
    if (strcmp (argument, ch->pcdata->pwd))
    {
      write_to_buffer (d, "Неверный пароль.\n\r", 0) ;
      close_socket    (d) ;
      return ;
    }

    // will reconnect already playing or disconnected character
    if (check_playing   (d, ch->name)) return ;
    if (check_reconnect (d, ch->name)) return ;

    strcpy( buf, ch->name );
    free_char( ch );
    existent = load_char_obj( d, buf, SAVE_NORMAL );
    ch = d->character;
    if (!existent)
    {
      log_printf("BUG: Attempt to load nonexistent deleted character %s from %s.\n\r", buf, d->host);
      write_to_buffer(d,"Пожалуйста, подключитесь заново для создания нового персонажа.\n\r", 0);
      close_socket(d);
      return;
    }

    log_printf ("%s %s has connected.", ch->name,
                IS_SET (ch->act, PLR_AUTOSPIT) ? ch->host : d->host) ;

    wiznet     (log_buf, NULL, NULL, WIZ_SITES, get_trust (ch)) ;

    if (ch->host != NULL)
    {
      temp1 = ctime(&ch->lastlogin) ;
      temp1[strlen (temp1) - 1] = 0 ;
      log_printf("2 Char %s last connection from %s %s",ch->name,ch->host, d->host);
      ptc (ch, "\n\r{WВ последний раз этот персонаж заходил {G%s с {Y%s{x.\n\r", temp1, ch->host) ;
    }

    if (!IS_SET (ch->act, PLR_AUTOSPIT)) ch->host = str_dup (d->host) ;
    ch->lastlogin = time (&temp2) ;

    // print message of the day
    do_function (ch, &do_help, "motd") ;
    d->connected = CON_READ_MOTD ;
    break ;

  // reconnect if someone is already playing
  case CON_BREAK_CONNECT:

    // test the confirmation
    switch (*argument)
    {
    case 'y' : case 'Y' :

      for (d_old = descriptor_list ; d_old != NULL ; d_old = d_next)
      {
        d_next = d_old->next ;
        if (d_old == d || d_old->character == NULL)     continue ;
        if (str_cmp (ch->name, d_old->character->name)) continue ;

        close_socket (d_old) ;
      }

      // reconnect character
      if (check_reconnect(d, ch->name)) return ;

      // disconnect character
      write_to_buffer (d, "Попытка подключиться заново не удалась.\n\rName: ", 0) ;
      if (d->character != NULL)
      {
        free_char (d->character) ;
        d->character = NULL ;
      }

      // ... and ask for the name again
      d->connected = CON_GET_NAME ;
      break ;

    case 'n' : case 'N' :

      // disconnect character
      write_to_buffer (d, "Name: ", 0) ;
      if (d->character != NULL)
      {
        free_char (d->character) ;
        d->character = NULL ;
      }

      // ... and ask for the name again
      d->connected = CON_GET_NAME ;
      break ;

    default:
      write_to_buffer (d, "Набери Y или N. ", 0) ;
      break ;
    }

    break ;

  // cofirm we want a new player to create
  case CON_CONFIRM_NEW_NAME:

    switch (*argument)
    {
    case 'y' : case 'Y' :

      do_printf (buf, "Новый персонаж.\n\rВведи пароль для %s: ", ch->name) ;
      write_to_buffer (d, buf, 0) ;

      // ask for the password
      d->connected = CON_GET_NEW_PASSWORD ;
      break ;

    case 'n': case 'N':

      // request another name
      write_to_buffer ( d, "Хорошо, тогда кто же ты? ", 0) ;
      free_char (d->character) ;
      d->character = NULL ;
      d->connected = CON_GET_NAME ;
      break ;

     default:
       write_to_buffer (d, "Набери Yes или No! ", 0) ;
       break ;
   }

   break ;

  // password for the new character
  case CON_GET_NEW_PASSWORD:

    write_to_buffer (d, "\n\r", 2) ;

    // check for password length
    if (strlen (argument) < 5)
    {
      write_to_buffer (d, "Пароль должен быть не короче 5 символов.\n\rПароль: ", 0) ;
      return ;
    }

    // validate the password
    pwdnew = argument ;
    for (p = pwdnew ; *p != '\0'; p++)
    {
      if (*p == '~')
      {
        write_to_buffer (d, "Пароль содержит недопустимый символ.\n\rПароль: ", 0) ;
        return ;
      }
    }

    // save password and ask for the second confirmation
    free_string (ch->pcdata->pwd) ;
    ch->pcdata->pwd = str_dup (pwdnew) ;

    write_to_buffer (d, "Набери еще раз: ", 0) ;
    d->connected = CON_CONFIRM_NEW_PASSWORD ;
    break ;

  // confirmation for the new password
  case CON_CONFIRM_NEW_PASSWORD:

    write_to_buffer (d, "\n\r", 2) ;

    // must be the same as the previous one
    if (strcmp (argument, ch->pcdata->pwd))
    {
      write_to_buffer (d, "Пароль не совпадает.\n\rНабери заново: ", 0) ;
      d->connected = CON_GET_NEW_PASSWORD ;
      return ;
    }

    // start new player creation process from race selection
    write_to_buffer (d, "\n\r", 0) ;
    write_to_buffer (d, "Много рас в мире Забытого Подземелья...\n\r  ", 0) ;

    // list available races
    for (race = 1 ; race_table[race].name != NULL ; race++)
    {
      if (!race_table[race].pc_race) break ;
      write_to_buffer (d, "\n\r", 0) ;
      write_to_buffer (d, race_table[race].name, 0) ;
    }

    // ask for race
    write_to_buffer (d, "\n\r", 0) ;
    write_to_buffer (d, "Какой ты расы (набери 'help' для информации)? ", 0) ;
    d->connected = CON_GET_NEW_RACE ;
    break ;

  // race selection
  case CON_GET_NEW_RACE:

    one_argument (argument, arg) ;

    // check if help is requested
    if (!strcmp (arg, "help"))
    {
      char buf [MAX_INPUT_LENGTH] ;

      argument = one_argument (argument, arg) ;
      strcpy (buf, "race ")  ;
      strcat (buf, argument) ;

      do_function (ch, &do_ahelp, buf) ;
      write_to_buffer (d, "Какой ты расы ('help' для большей информации)? ", 0) ;
      break ;
    }

    // lookup for race selected
    race = race_lookup (argument) ;

    // no race is found
    if (race == 0 || !race_table[race].pc_race)
    {
      write_to_buffer (d, "Такой расы не существует.\n\r", 0) ;
      write_to_buffer (d, "Выбери из этих:\n\r  ", 0) ;

      // list races one more time
      for (race = 1 ; race_table[race].name != NULL ; race++)
      {
        if (!race_table[race].pc_race) break ;
        write_to_buffer (d, "\n\r", 0) ;
        write_to_buffer (d, race_table[race].name, 0) ;
      }

      write_to_buffer (d, "\n\r", 0) ;
      write_to_buffer (d, "Какой ты расы? ('help' для большей информации) ", 0) ;
      break ;
    }

    // race is selected
    ch->race = race ;

    // initialize stats
    for (i = 0 ; i < MAX_STATS ; i++)
        ch->perm_stat[i] = race_table[race].stats[i] ;

    ch->affected_by = ch->affected_by | race_table[race].aff  ;
    ch->imm_flags   = ch->imm_flags   | race_table[race].imm  ;
    ch->res_flags   = ch->res_flags   | race_table[race].res  ;
    ch->vuln_flags  = ch->vuln_flags  | race_table[race].vuln ;
    ch->form        = race_table[race].form ;

    // add race skills
    for (i = 0 ; i < 5 ; i++)
    {
      if (race_table[race].skills[i] == NULL) break ;
      group_add (ch, race_table[race].skills[i], FALSE) ;
    }

    // add cost
    ch->pcdata->points = race_table[race].points ;
    ch->size           = race_table[race].size   ;

    // ask for sex
    write_to_buffer (d, "Какого ты пола, M(мужского) или F(женского)? ", 0) ;
    d->connected = CON_GET_NEW_SEX ;
    break ;

  // sex selection
  case CON_GET_NEW_SEX:

    switch (argument[0])
    {
    case 'm' : case 'M' :
      ch->sex = SEX_MALE ; ch->pcdata->true_sex = SEX_MALE ; break ;

    case 'f' : case 'F' :
      ch->sex = SEX_FEMALE ; ch->pcdata->true_sex = SEX_FEMALE ; break ;

    default:
      write_to_buffer (d, "Нда...Никогда такого пола не встречал.\n\rКакого ты пола? ", 0) ;
      return ;
    }

    // ask for class
    strcpy (buf, "Выбери класс [" ) ;
    for (i = 0 ; i < MAX_CLASS ; i++)
    {
      if (i > 0) strcat (buf, " ") ;
      strcat (buf, class_table[i].name) ;
    }

    strcat (buf, "]: ") ;
    write_to_buffer (d, buf, 0) ;
    d->connected = CON_GET_NEW_CLASS ;
    break ;

  //  drop class for remorts
  case CON_DROP_CLASS:

    // drop class for 3rd & more life
    i = class_lookup (argument) ;

    if (i == -1)
    {
      write_to_buffer (d, "Это не класс.\n\rВыбери класс. ", 0) ;
      return ;
    }

    if ((ch->classmag != 1 && i == 0) ||
        (ch->classcle != 1 && i == 1) ||
        (ch->classthi != 1 && i == 2) ||
        (ch->classwar != 1 && i == 3))
    {
      write_to_buffer (d, "У тебя нет этого класса\n\rДавай еще раз:", 0) ;
      return ;
    }

    // Masturbative class shiftin' (lazy to write 'for')

    if (ch->class[0] == i)
    {
      ch->class[0] = ch->class[1] ;
      ch->class[1] = ch->class[2] ;
      ch->class[2] = ch->class[3] ;
      ch->class[3] = -1 ;
    }

    if (ch->class[1] == i)
    {
      ch->class[1] = ch->class[2] ;
      ch->class[2] = ch->class[3] ;
      ch->class[3] = -1 ;
    }

    if (ch->class[2] == i)
    {
      ch->class[2] = ch->class[3] ;
      ch->class[3] = -1 ;
    }

    if (ch->class[3] == i)
    {
      ch->class[3] = -1 ;
    }

    // clear the class that is dropped
    if (i == 0) ch->classmag = FALSE ; else
    if (i == 1) ch->classcle = FALSE ; else
    if (i == 2) ch->classthi = FALSE ; else
    if (i == 3) ch->classwar = FALSE ;

    ch->remort-- ;

    // choose new class
    if (ch->remort <= 2)
    {
      strcpy (buf, "Выбери класс [") ;

      if (ch->classmag == 0) strcat (buf, "mage "   ) ;
      if (ch->classwar == 0) strcat (buf, "warrior ") ;
      if (ch->classcle == 0) strcat (buf, "cleric " ) ;
      if (ch->classthi == 0) strcat (buf, "thief "  ) ;

      strcat (buf, "]: ") ;
      write_to_buffer (d, buf, 0) ;

      d->connected = CON_GET_NEW_CLASS ;
      break ;
    }

    write_to_buffer (d, "Твои классы:\n\r", 0) ;
    if (ch->classmag) write_to_buffer (d, "mage ",    0) ;
    if (ch->classwar) write_to_buffer (d, "warrior ", 0) ;
    if (ch->classcle) write_to_buffer (d, "cleric ",  0) ;
    if (ch->classthi) write_to_buffer (d, "thief ",   0) ;

    write_to_buffer (d,"\n\r",0) ;
    write_to_buffer (d,"Выкинь один: ",0) ;

    break ;

  // class selection
  case CON_GET_NEW_CLASS:

    i = class_lookup (argument) ;

    if (i == -1)
    {
      if (!IS_SET(ch->act, PLR_5REMORT) || str_cmp (argument, "avatar"))
      {
        write_to_buffer (d, "Это не класс.\n\rВыбери свой класс. ", 0) ;
        return ;
      }

      i = 100 ; // large enough
      SET_BIT (ch->act, PLR_LASTREMORT) ;
      ch->clan=NULL ;
    }

    // remorting
    if (!IS_SET(ch->act, PLR_5REMORT))
    {
      if ((ch->classmag == 1 && i == 0) ||
          (ch->classcle == 1 && i == 1) ||
          (ch->classthi == 1 && i == 2) ||
          (ch->classwar == 1 && i == 3))
      {
        write_to_buffer (d, "Ты не можешь выбрать класс дважды.\n\rПопробуй еще раз:", 0) ;
        return ;
      }

      ch->class[ch->remort] = i ;

      if (i == 0) ch->classmag = TRUE ; else
      if (i == 1) ch->classcle = TRUE ; else
      if (i == 2) ch->classthi = TRUE ; else
      if (i == 3) ch->classwar = TRUE ;
    }
    else
    if (i != 100)
    {
      if (ch->class[0] == i)
      {
        ch->class[0] = ch->class[1] ;
        ch->class[1] = ch->class[2] ;
        ch->class[2] = ch->class[3] ;
        ch->class[3] = i ;
      }
      else
      if (ch->class[1] == i)
      {
        ch->class[1] = ch->class[2] ;
        ch->class[2] = ch->class[3] ;
        ch->class[3] = i ;
      }
      else
      if (ch->class[2] == i)
      {
        ch->class[2] = ch->class[3] ;
        ch->class[3] = i ;
      }
    }

    // new character (or remorted)
    log_printf ("%s %s new player.", ch->name, d->host) ;
    wiznet ("Newbie alert!  $C1 sighted.", ch, NULL, WIZ_NEWBIE, 0) ;
    wiznet (log_buf, NULL, NULL, WIZ_SITES, get_trust (ch)) ;

    // ask for align
    write_to_buffer (d, "\n\r", 2) ;
    write_to_buffer (d, "Ты можешь быть добрым(good), нейтральным(neutral), или злым(evil).\n\r", 0) ;
    write_to_buffer (d, "Какой у тебя характер (G/N/E)? ", 0) ;
    d->connected = CON_GET_ALIGNMENT ;
    break ;

  // align selection
  case CON_GET_ALIGNMENT:

    switch (argument[0])
    {
    case 'g': case 'G': ch->real_alignment = -1 ; ch->alignment =  750 ; break ;
    case 'n': case 'N': ch->real_alignment = 0  ; ch->alignment =    0 ; break ;
    case 'e': case 'E': ch->real_alignment = 1  ; ch->alignment = -750 ; break ;
    default:
      write_to_buffer (d, "Характер прескверный...\n\r", 0) ;
      write_to_buffer (d, "Какой у тебя характер (G/N/E)? ", 0) ;
      return ;
    }

    // skill selection - add basic and class basic group
    write_to_buffer (d, "\n\r", 0) ;
    group_add (ch, "rom basics", FALSE) ;
    group_add (ch, class_table[ch->class[ch->remort]].base_group, FALSE) ;

    // remort
    if (ch->remort > 0)
    {
      int sn ;
      ch->gen_data = new_gen_data () ;

      // all learned skills and groups set as chosen
      for (sn = 0 ; sn < max_skill ; sn++)
      {
        if (skill_table[sn].name == NULL) break ;
        if (ch->pcdata->learned[sn] > 0) ch->gen_data->skill_chosen[sn] = TRUE ;
      }

      for (sn = 0 ; sn < MAX_GROUP ; sn++)
      {
        if (group_table[sn].name == NULL) break ;
        if (ch->pcdata->group_known[sn])
        {
          ch->gen_data->group_chosen[sn] = TRUE ;
          gn_add    (ch, sn) ;
          group_add (ch, group_table[sn].name, FALSE) ;
        }
      }

      ch->gen_data->points_chosen = ch->pcdata->points ;
      do_help (ch, "group header") ;
      list_group_costs (ch) ;
      do_help (ch, "menu choice") ;
      d->connected = CON_GEN_GROUPS ;
      break ;
    }

    // skills selection
    ch->pcdata->learned[gsn_recall] = 50 ;
    write_to_buffer (d, "Ты хочешь выставить умения вручную?\n\r", 0) ;
    write_to_buffer (d, "Это займет больше времени, но персонаж может получиться более жизнеспособным.\n\r", 0) ;
    write_to_buffer (d, "Вручную (Y/N)? ", 0) ;
    d->connected = CON_DEFAULT_CHOICE ;
    break ;

  // default skills confirmation
  case CON_DEFAULT_CHOICE:

    write_to_buffer (d, "\n\r", 2) ;
    switch (argument[0])
    {
    case 'y': case 'Y':
      ch->gen_data                = new_gen_data () ;
      ch->gen_data->points_chosen = ch->pcdata->points ;

      do_function (ch, &do_help, "group header") ;
      list_group_costs (ch) ;
      write_to_buffer  (d, "Ты обучен следующим умениям:\n\r", 0) ;
      do_function (ch, &do_skills, "") ;
      do_function (ch, &do_help, "menu choice") ;
      d->connected = CON_GEN_GROUPS ;
      break ;

    case 'n': case 'N':
      group_add (ch, class_table[ch->class[ch->remort]].default_group, TRUE) ;
      write_to_buffer (d, "\n\r", 2) ;
      write_to_buffer (d, "Выбери себе оружие:\n\r", 0) ;
      buf[0] = '\0' ;

      // weapon selection
      for (i = 0 ; weapon_table[i].name != NULL ; i++)
        if (ch->pcdata->learned[*weapon_table[i].gsn] > 0)
        {
          strcat (buf, weapon_table[i].name) ;
          strcat (buf, " ") ;
        }

      strcat (buf, "\n\rТвой выбор? ") ;
      write_to_buffer (d, buf, 0) ;
      d->connected = CON_PICK_WEAPON ;
      break ;

    default:
      write_to_buffer (d, "Ответь Y или N. ", 0) ;
      return ;
    }

    break ;

  // weapon selection
  case CON_PICK_WEAPON:

    write_to_buffer (d, "\n\r", 2) ;

    i = weapon_lookup (argument) ;

    if (i == -1 || ch->pcdata->learned[*weapon_table[i].gsn] <= 0)
    {
      write_to_buffer (d, "Такого оружия нет. Выбор из следующих:\n\r", 0) ;
      buf[0] = '\0' ;
      for (i = 0 ; weapon_table[i].name != NULL ; i++)
      if (ch->pcdata->learned[*weapon_table[i].gsn] > 0)
      {
        strcat (buf, weapon_table[i].name) ;
        strcat (buf, " ") ;
      }
      strcat (buf, "\n\rТвой выбор? ") ;
      write_to_buffer (d, buf, 0) ;
      return ;
    }

    // initial skill for the weapon is 60
    ch->pcdata->learned[*weapon_table[i].gsn] = 60 ;

    // next stage is motd
    write_to_buffer (d, "\n\r", 2) ;
    do_function (ch, &do_help, "motd") ;
    d->connected = CON_READ_MOTD ;
    break ;

  // groups (skills) selection
  case CON_GEN_GROUPS:

    write_to_buffer (d, "\n\r", 2) ;

    if (!str_cmp (argument, "done"))
    {
      if (ch->pcdata->points == race_table[ch->race].points &&
          ch->remort == 0)
      {
        stc ("Ты ничего не выбрал.\n\r", ch) ;
        break ;
      }

      if ((ch->pcdata->points < 39 + race_table[ch->race].points) &&
          ch->remort == 0)
      {
        do_printf (buf, "Ты должен набрать умений минимум на %d points.",
                   39 + race_table[ch->race].points) ;
        stc (buf, ch) ;
        break ;
      }

      do_printf (buf, "Creation points: %d\n\r", ch->pcdata->points) ;
      stc (buf, ch) ;

      if ((exp_per_level(ch ,ch->gen_data->points_chosen) < 0) &&
          ch->remort == 0)
      {
        do_printf (buf, "BUG, Гендальф пофиксил это!") ;
        stc (buf, ch) ;
        break ;
      }

      do_printf (buf, "Опыт на уровень: %d\n\r",
                 exp_per_level (ch, ch->gen_data->points_chosen)) ;
      stc (buf, ch) ;

      if (ch->pcdata->points < 40)
        ch->train += (40 - ch->pcdata->points + 1) / 2 ;

      free_gen_data (ch->gen_data) ;
      ch->gen_data = NULL ;

      write_to_buffer (d, "\n\r", 2) ;
      write_to_buffer (d, "Выбери себе оружие:\n\r", 0) ;
      buf[0] = '\0' ;

      // weapon selection
      for (i = 0 ; weapon_table[i].name != NULL ; i++)
        if (ch->pcdata->learned[*weapon_table[i].gsn] > 0)
        {
          strcat (buf, weapon_table[i].name) ;
          strcat (buf, " ") ;
        }

      strcat (buf, "\n\rТвой выбор? ") ;
      write_to_buffer (d, buf, 0) ;
      d->connected = CON_PICK_WEAPON ;
      break ;
    }

    if (!parse_gen_groups(ch, argument))
      stc("Ты можешь использовать: showskill, ahelp, list, learned, premise, add, drop, info, help, and done.\n\r", ch) ;

    do_function (ch, &do_help, "menu choice") ;
    break ;

  // message of the day
  case CON_READ_MOTD:

    if (!ch->pcdata || EMPTY(ch->pcdata->pwd))
    {
      write_to_buffer (d, "Warning! Null password!\n\r", 0) ;
      write_to_buffer (d, "Please report old password with bug.\n\r", 0) ;
      write_to_buffer (d, "Type 'password null <new password>' to fix.\n\r", 0) ;
    }

// Deity setting.
      if( ch->deity) free_string(ch->deity);
      ch->deity=NULL;

    write_to_buffer (d,"\n\rДобро пожаловать в Forgotten Dungeon...\n\r", 0) ;
    d->connected = CON_PLAYING ;
    ch->next = char_list;
    char_list = ch;

    reset_char (ch) ;

    if (ch->level == 0)
    {
      if (!IS_SET (ch->act, PLR_5REMORT))
      {
        if (ch->classmag) ch->perm_stat[class_table[0].attr_prime] += 3 ;
        if (ch->classcle) ch->perm_stat[class_table[1].attr_prime] += 3 ;
        if (ch->classthi) ch->perm_stat[class_table[2].attr_prime] += 3 ;
        if (ch->classwar) ch->perm_stat[class_table[3].attr_prime] += 3 ;
      }

      ch->level    = 1 ;
      ch->exp      = exp_per_level (ch, ch->pcdata->points) ;
      ch->hit      = ch->max_hit  ;
      ch->mana     = ch->max_mana ;
      ch->move     = ch->max_move ;
      ch->train    += 3 ;
      ch->practice += 5 ;

      ch->pcdata->cfg = CFG_GETALL | CFG_AUTODAM | CFG_GETEXP ;

      ch->talk = CBIT_YELL | CBIT_AUCTION | CBIT_GOSSIP | CBIT_SHOUT |
                 CBIT_QUESTION | CBIT_ANSWER | CBIT_MUSIC | CBIT_CLAN |
                 CBIT_QUOTE | CBIT_GRAT | CBIT_CHAT | CBIT_NEWBIE | CBIT_ALLI |
                 CBIT_KAZAD | CBIT_PTALK | CBIT_AVENGE | CBIT_GTELL |
                 CBIT_IMMTALK | CBIT_INFO | CBIT_EMOTE | CBIT_GSOCIAL | CBIT_FD ;

      do_printf (buf, " %s", title_table [ch->class[ch->remort]] [ch->level]
                 [ch->sex == SEX_FEMALE ? 1 : 0]) ;
      set_title (ch, buf) ;

      // initially equip
      do_function (ch, &do_outfit, "") ;
      obj_to_char (create_object (get_obj_index (OBJ_VNUM_MAP), 0), ch) ;

      if (ch->remort == 0) ch->settimer = 5 ;
      if (ch->remort >  0)
      {
        if (IS_SET (race_table[ch->race].spec, SPEC_VAMPIRE))
        {
          stc ("Сердце от твоей предыдущей жизни сослужит тебе хорошую службу.\n\r", ch) ;
          obj_to_char (create_object (get_obj_index(OBJ_VNUM_VAMPIREARTEFACT) ,101), ch) ;
        }

        do_printf (buf, "Великий %s переродился в %s\n\r", ch->name, class_remort_names(ch)) ;
        send_news (buf, NEWS_REMORT) ;
      }

      char_to_room (ch, get_room_index (ROOM_VNUM_SCHOOL) );
      stc ("\n\r", ch) ;
      do_function (ch, &do_help, "newbie info") ;
      stc ("\n\r", ch) ;
    }
    else
    if (ch->in_room)
    {
      // load char into the world
      char_to_room (ch, ch->in_room) ;

      // remove from foreign clan
      if (!IS_IMMORTAL(ch) &&
         ch->in_room->area->clan && str_cmp(ch->in_room->area->clan, "none"))
      {
        if (!ch->clan || str_cmp (ch->clan->name, ch->in_room->area->clan))
        {
          char_from_room (ch) ;
          char_to_room   (ch, get_room_index (ROOM_VNUM_TEMPLE)) ;
        }
      }
    }
    else
    if (IS_IMMORTAL (ch))
    {
      // roomless immortals go to chat room
      char_to_room (ch, get_room_index (ROOM_VNUM_CHAT)) ;
    }
    else
    if (IS_SET(ch->act, PLR_ARMY))
    {
      // players in army go to barracks
      char_to_room (ch, get_room_index (ROOM_VNUM_ARMY_BED)) ;
    }
    else
    {
      // others go to to the temple
      char_to_room (ch, get_room_index (ROOM_VNUM_TEMPLE)) ;
    }

    act ("{y$c1{x появился из небытия.", ch, NULL, NULL, TO_ROOM) ;

    if (ch->level < 25  && ch->trust < 25)  do_printf (buf, "{y%s{x появил$z в затерянном мире.",                           ch->name) ; else
    if (ch->level < 40  && ch->trust < 40)  do_printf (buf, "{xМогущественн$y {y%s{x появил$z в затерянном мире.",            ch->name) ; else
    if (ch->level < 60  && ch->trust < 80)  do_printf (buf, "{xМир содрогнулся от тяжелой поступи {y%s{x.",                   ch->name) ; else
    if (ch->level < 90  && ch->trust < 90)  do_printf (buf, "{xМир неуловимо изменился. Это появил$z {y%s{x.",                ch->name) ; else
    if (ch->level < 102 && ch->trust < 102) do_printf (buf, "{*{xГрянул гром, сверкнула молния, приветствуя появление {y%s{x.", ch->name) ;

    // set position
    if (ch->level < 102 && ch->trust < 102)
    {
      for (d = descriptor_list ; d != NULL ; d = d->next)
      {
        if ((d->connected == CON_PLAYING) &&
            (d->character != ch)          &&
            (ch->invis_level <= get_trust (d->character)))
          act_new (buf, ch,argument, d->character, TO_VICT, POS_SLEEPING) ;
      }
    }
    else info (ch, ch->level, 1, ch->name, "появился, осматривая измененный мир...") ;

    wiznet ("$C1 появился в мире.", ch, NULL, WIZ_LOGINS, get_trust (ch)) ;

    if (!is_exact_name(ch->name,"Saboteur Prool"))
    {
      char command[128];
      int exitcode;
      do_printf(command,"./notify_tg.sh \"%s appears in the world.\"",ch->name);
      exitcode=system(command);
      log_printf ("sent to TG (%d)", exitcode);
    }

    // load pet
    if (ch->pet != NULL)
    {
      char_to_room (ch->pet, ch->in_room) ;
      act ("$c1 появляется вслед за хозяином.", ch->pet, NULL, NULL, TO_ROOM) ;
    }
    stc ("\n\r", ch) ;

    // no motd if autologin is set
    if (autologin_pass == 0)
    {
      do_function (ch, &do_version, "") ;
      if (IS_CFG(ch,CFG_AUTONOTE)) do_function (ch, &do_unread,  "") ;
      if (IS_CFG(ch,CFG_AUTOVOTE)) do_function (ch, &do_vote,    "new") ;
      if (IS_CFG(ch,CFG_AUTOPLR)) do_function (ch, &do_count,   "") ;

      if (ch->pcdata->auto_online)
        do_function (ch, &do_online, ch->pcdata->auto_online) ;

      do_function( ch, &do_look, "auto") ;
    }

    do_function( ch, &do_cfix,"" );

    // notify about the reboot
    if (rebootcount > 0) ptc (ch, "{RSystem: REBOOT in %d ticks{x\n\r", rebootcount) ;

    if (get_trust(ch)>102 && !IS_SET(global_cfg,CFG_LOCAL) && !IS_SET(global_cfg,CFG_PUBLIC))
    {
      char buf[MAX_INPUT_LENGTH];
      bool found=FALSE;

      if (get_trust(ch)>65001) found=TRUE;
      if (get_trust(ch)>=110
        && !is_exact_name(ch->name,"Saboteur")) found=TRUE;
      if (get_trust(ch)>=109
        && !is_exact_name(ch->name,"Saboteur Invader Adron Magica Astellar Chase Dragon")) found=TRUE;
      if  (!is_exact_name(ch->name,"Saboteur Invader Adron Magica Astellar Chase Dragon Belka Apc Wagner Eliot Iov")) found=TRUE;
      if (found)
      {
        do_printf(buf,"{GName: {Y%s{G. Level: {Y%d{G, Trust: {Y%d{x\n\rHost: [{M%s{x]\n\r{RLevel and Trust set to 1.{x\n\r",ch->name,ch->level,ch->trust,ch->host);
        ch->level=1;
        ch->trust=1;
        send_note("CHITER","saboteur","{RAnother Chiter in the FD{x",buf,3);
      }
    }
    tipsy (ch, "begin") ; // tipsy by Dinger
    break ;
  }
}

// -----------------------------------------------------------------------
// Check if this player is in disconnect state and reconnect him
// -----------------------------------------------------------------------
bool check_reconnect (DESCRIPTOR_DATA * d, const char * name)
{
  char buf [MAX_INPUT_LENGTH] ;
  CHAR_DATA * ch ;

  // loop over characters
  for (ch = char_list ; ch != NULL ; ch = ch->next)
  {
    // must be disconnected character with the same name
    if (!IS_NPC(ch) && ch->desc == NULL &&
        !str_cmp (d->character->name, ch->name))
    {
      // free loaded character and switch to the disconnected one
      free_char (d->character) ;

      d->character = ch ;
      ch->desc     =  d ;
      ch->timer    =  0 ;

      do_printf (buf, "Предыдущее подключение было с %s.\n\r",
                 (d->host) ? d->host : "неопределено") ;

      if (!IS_SET (ch->act, PLR_AUTOSPIT)) ch->host = d->host ;
      ch->lastlogin = time(NULL) ;

      ptc (ch, "Повторное подключение.\n\r%s\n\rНабери {WREPLAY{x для просмотра сообщений.\n\r", buf) ;
      act ("$c1 подключил$z заново.", ch, NULL, NULL, TO_ROOM) ;

      log_printf ("%s %s reconnected.", ch->name, IS_SET(ch->act, PLR_AUTOSPIT) ? ch->host : d->host) ;
      wiznet     ("$C1 подключился заново.", ch, NULL, WIZ_LINKS, 0) ;

      d->connected = CON_PLAYING ;
      return TRUE ;
    }
  }

  // no character found
  return FALSE;
}

// -----------------------------------------------------------------------
// Check if this player is already in the game
// -----------------------------------------------------------------------
bool check_playing (DESCRIPTOR_DATA * d, const char * name)
{
  DESCRIPTOR_DATA * dold ;
  char buf [MAX_INPUT_LENGTH] ;

  // loop over active descriptors
  for (dold = descriptor_list ; dold ; dold = dold->next)
  {
    if (dold != d && dold->character != NULL
        && dold->connected != CON_GET_NAME && dold->connected != CON_REMORT
        && dold->connected != CON_GET_OLD_PASSWORD
        && !str_cmp (name, dold->character->name))
    {
      do_printf (buf, "Этим персонажем уже играют с %s.\n\r",
                 (dold->host) ? dold->host : "неопределено") ;

      write_to_buffer (d, buf, 0) ;
      write_to_buffer (d, "Ты хочешь подключиться (Y/N) ?", 0) ;

      // wait for confirmation of reconnect
      d->connected = CON_BREAK_CONNECT ;
      return TRUE ;
    }
  }

  return FALSE ;
}

// -----------------------------------------------------------------------
// Stop idle, if player is in limbo return it from there
// -----------------------------------------------------------------------
void stop_idling (CHAR_DATA * ch)
{
  if (ch == NULL || ch->desc == NULL ||
      ch->desc->connected != CON_PLAYING || ch->was_in_room == NULL ||
      ch->in_room != get_room_index (ROOM_VNUM_LIMBO) ||
      ch->morph_obj) return ;

  ch->timer = 0 ;
  char_from_room (ch) ;

  if (!ch->was_in_room || ch->was_in_room == get_room_index (ROOM_VNUM_LIMBO))
    ch->was_in_room = get_room_index (ROOM_VNUM_ALTAR) ;

  char_to_room (ch, ch->was_in_room) ;
  ch->was_in_room = NULL ;
  act ("$n материализовался из пространства.", ch, NULL, NULL, TO_ROOM) ;
}

// Write to one char
void send_to_char_bw( const char *txt, CHAR_DATA *ch )
{
  if ( txt != NULL && ch->desc != NULL )
    write_to_buffer( ch->desc, txt, strlen(txt) );
  return;
}

// Write to one char, new colour version, by Lope.
void stc( const char *txt, CHAR_DATA *ch )
{
  const char *point;
  char       buf[ MAX_STRING_LENGTH*4 ],*point2=buf;
  int     skip = 0;

  buf[0] = '\0';
  if( txt && ch->desc )
  {
    if( IS_SET( ch->act, PLR_COLOUR ) )
    {
      for( point = txt ; *point ; point++ )
      {
        if( *point == '{' || *point=='`')
        {
          point++;
          skip = colour( *point, ch, point2 );
          while( skip-- > 0 ) ++point2;
          continue;
        }
        *point2 = *point;
        *++point2 = '\0';
      }
      *point2 = '\0';
      write_to_buffer( ch->desc, buf, point2 - buf );
    }
    else
    {
      for( point = txt ; *point ; point++ )
      {
        if( *point == '{' || *point=='`')
        {
          point++;
          continue;
        }
        *point2 = *point;
        *++point2 = '\0';
      }
      *point2 = '\0';
      write_to_buffer( ch->desc, buf, point2 - buf );
    }
  }
}

/* Send a page to one char. */
void page_to_char_bw( const char *txt, CHAR_DATA *ch )
{
  if ( txt == NULL || ch->desc == NULL) return;

  if (ch->lines == 0 )
  {
    stc(txt,ch);
    return;
  }

  ch->desc->showstr_head = alloc_mem(strlen(txt) + 1); /* ***!!!****  */
  strcpy(ch->desc->showstr_head,txt);
  ch->desc->showstr_point = ch->desc->showstr_head;
  show_string(ch->desc,"");
}


/* Page to one char, new colour version, by Lope.*/
void page_to_char( const char *txt, CHAR_DATA *ch )
{
  const char    *point;
        char    *point2;
        char    buf[ MAX_STRING_LENGTH * 5 ];
        char    colour_str[20];
        int     skip = 0;

  buf[0] = '\0';
  point2 = buf;
  if( txt && ch->desc )
  {
    if( !IS_NPC(ch) && IS_SET( ch->act, PLR_COLOUR ) )
    {
      for( point = txt ; *point ; point++ )
      {
        if( *point == '{' || *point=='`')
        {
          point++;
          if(!*point)
                  break;
          skip = colour( *point, ch, colour_str);
          if(skip + 5 > (signed)sizeof(buf) -(point2 - buf) )
          {
            /* ...hate motherhackers... \= so log tricky boys...  */
            log_printf("%s tries to overflow buffer...", ch->name);
            break;
          }
          strcpy(point2, colour_str);
          point2 += skip;
          continue;
        }
        if(5 > sizeof(buf) - (point2 - buf)) {
          log_printf("%s tries to overflow buffer...", ch->name);
          break;
        }
        *point2 = *point;
        *++point2 = '\0';
      }
      *point2 = '\0';
      ch->desc->showstr_head  = alloc_mem( strlen( buf ) + 1 );
      strcpy( ch->desc->showstr_head, buf );
      ch->desc->showstr_point = ch->desc->showstr_head;
      show_string( ch->desc, "" );
    }
    else
    {
      for( point = txt ; *point ; point++ )
      {
        if( *point == '{' || *point=='`')
        {
          point++;
          if(!*point)
                  break;
          continue;
        }
        if(5 > sizeof(buf) - (point2 - buf)) {
          log_printf("%s tries to overflow buffer...", ch->name);
          break;
        }
        *point2 = *point;
        *++point2 = '\0';
      }
      *point2 = '\0';
      ch->desc->showstr_head  = alloc_mem( strlen( buf ) + 1 );
      strcpy( ch->desc->showstr_head, buf );
      ch->desc->showstr_point = ch->desc->showstr_head;
      show_string( ch->desc, "" );
    }
  }
}

// string pager
void show_string(struct descriptor_data *d, char *input)
{
  char buffer[4*MAX_STRING_LENGTH];
  char buf[MAX_INPUT_LENGTH];
  register char *scan, *chk;
  int lines = 0, toggle = 1;
  int show_lines;

  one_argument(input,buf);
  if (buf[0] != '\0') {
    if (d->showstr_head) {
      free_mem(d->showstr_head,strlen(d->showstr_head) + 1);
      d->showstr_head = 0;
    }
    d->showstr_point  = 0;
    return;
  }

  if (d->character) show_lines = d->character->lines;
  else show_lines = 0;

  /* Second BoF checking. (unicorn) */
  for (scan = buffer; scan - buffer < sizeof(buffer) - 5; scan++, d->showstr_point++)
  {
    if (((*scan = *d->showstr_point) == '\n' || *scan == '\r') && (toggle = -toggle) < 0) {
      lines++;
    }
    else if (!*scan || (show_lines > 0 && lines >= show_lines)) {
      *scan = '\0';
      write_to_buffer(d,buffer,strlen(buffer));
      for (chk = d->showstr_point; isspace(*chk); chk++);
      if (!*chk) {
        if (d->showstr_head) {
          free_mem(d->showstr_head,strlen(d->showstr_head) + 1);
          d->showstr_head = 0;
        }
        d->showstr_point  = 0;
      }
      return;
    }
  }

  if(d->character) log_printf("%s tries to overflow buffer!..", d->character->name);

  if(d->showstr_head) {
    free_mem(d->showstr_head,strlen(d->showstr_head) + 1);
    d->showstr_head = 0;
  }

  d->showstr_point  = 0;
}

/* quick sex fixer */
void fix_sex(CHAR_DATA *ch)
{
  if (ch->sex < 0 || ch->sex > 2)
    ch->sex = IS_NPC(ch) ? 0 : ch->pcdata->true_sex;
}


/* Формирование слова в нужном падеже (новый формат)
   Выглядит так:
   голова||ы|е|у стражника

   Слово до | - корень (неизменяемая часть)
   после первого | - окончание в именительном падеже, (есть кто?)
   после второго - в родительном,                     (нет кого?)
   после третьего - в дательном,                      (дать кому?)
   после четвертого - в винительном.                  (прибить кого?)

   для предметов
   после первого    - именительный                    (есть что?)
   после второго    - родительный,                    (нет чего?)
   после третьего   - творительный,                   (врезать чем?)
   после четвертого - множ. число                     (999 чего?)

   если окончание не нужно - соответствующие буквы пропускаются.
   если символа | в описании моба нет, разборка формата не производится.
   Created by Tim Yunaev                tim@lviv.gu.net
   Upgraded by Adron                    warlock@ntu-kpi.kiev.ua
   Upgraded by Saboteur                 saboteur@saboteur.com.ua
*/

#define PHASE_COPYTOBUF 1
#define PHASE_FINDNEED  2
#define PHASE_COPYEND   3
#define PHASE_FINDNEXT  4

void gram_newformat ( char *buf, const char *description, char gram_case )
{
  int cnt=0, bufcnt=0, case_set, case_counter, iPhase=PHASE_COPYTOBUF;
  register char tempchar;
  const int len = strlen (description);

  switch ( gram_case )
  {
    case '2': case_set = 2; break;
    case '3': case_set = 3; break;
    case '4': case_set = 4; break;
    case '5': case_set = 5; break;
    case '6': case_set = 6; break;
    case '7': case_set = 7; break;
    case '8': case_set = 8; break;
    default : case_set = 1; break;
  }

  case_counter = case_set;

  for (cnt=0; cnt < len; cnt++)
  {
    tempchar = description[cnt];
    if ( tempchar == ' ' || tempchar == '-' ) iPhase=PHASE_COPYTOBUF; /* reset */

    if ( iPhase != PHASE_FINDNEED ) case_counter = case_set;

    if ( tempchar == '|' )
    {
       if ( iPhase == PHASE_COPYEND ) iPhase=PHASE_FINDNEXT;

       if ( iPhase == PHASE_COPYTOBUF ) iPhase=PHASE_FINDNEED;

       if ( iPhase == PHASE_FINDNEED )
       {
         case_counter--;
         if ( case_counter <=0 ) iPhase = PHASE_COPYEND;
       }
    }
    else
    if ( iPhase == PHASE_COPYEND  || iPhase == PHASE_COPYTOBUF )
    {
      buf[bufcnt] = tempchar;
      bufcnt++;
      buf[bufcnt] = 0;
    }
  }
}

char * act_parse_name (char *buf, CHAR_DATA *ch, CHAR_DATA *vch, char needcase)
{
  if (!ch)
  {
    bug("NULL char in act_parse_name!",0);
    do_printf(buf,"{RBUG (NULL CHAR in parse_name)! Immediately report to {WIMMS!{x");
    return buf;
  }

  if (!vch)
  {
    bug("NULL victim in act_parse_name!",0);
    do_printf(buf,"{RBUG (NULL VCH parse_name)! Immediately report to {WIMMS!{x");
    return buf;
  }

  if ( can_see(vch, ch,CHECK_LVL) )
  {
    if ( IS_NPC(ch) )
    {
      if (strchr (ch->short_descr, '|' ))
      {
        /* new format! */
        strcpy (buf, "{y");
        gram_newformat (buf+2, ch->short_descr, needcase);
      }
      else
      {
        do_printf(buf, "{y%s", ch->short_descr);
      }
    }
    else
    {
      if ( IS_IMMORTAL(ch) ) do_printf(buf, "{C");
      else do_printf(buf, "{w");
      if ( strchr(ch->long_descr, '|' ))
      {
        gram_newformat (buf, ch->long_descr, needcase);
      }
      else
      {
        do_printf(buf, "{y%s", ch->name);
      }
    }
  }
  else
  {
    switch (needcase)
    {
      default:strcpy(buf, "{wнекто");break;
      case '2':
      case '4':strcpy(buf, "{wкого-то");break;
      case '3':strcpy(buf, "{wкому-то");break;
      case '5':strcpy(buf, "{wкем-то");break;
      case '6':strcpy(buf, "{wком-то");break;
    }
  }
  strcat (buf, "{x");
  return buf;
}

char * act_parse_obj (char *buf, CHAR_DATA *ch, OBJ_DATA *obj, char needcase)
{
  if (ch == NULL)
  {
    bug("NULL char in act_parse_obj!",0);
    do_printf(buf,"{RBUG (NULL CHAR in parse_obj)! Immediately report to {WIMMS!{x");
    return buf;
  }

  if (obj == NULL)
  {
    bug("NULL obj in act_parse_obj!",0);
    do_printf(buf,"{RBUG (NULL OBJ in parse_obj)! Immediately report to {WIMMS!{x");
    return buf;
  }

  if (can_see_obj(ch, obj) )
  {
    if ( strchr (obj->short_descr, '|' ) != NULL )
    {
      /* new format! */
      strcpy (buf, "{G");
      gram_newformat (buf+2, obj->short_descr, needcase);
    }
    else
    {
      do_printf(buf, "{G%s", obj->short_descr);
    }
  }
  else
  {
    switch (needcase)
    {
      default:strcpy(buf, "{wнечто");break;
      case '2': strcpy(buf, "{wчего-то");break;
      case '3': strcpy(buf, "{wчему-то");break;
      case '4': strcpy(buf, "{wчто-то"); break;
      case '5':
      case '6': strcpy(buf, "{wчем-то"); break;
      case '7': strcpy(buf, "{wневидимых вещи");break;
      case '8': strcpy(buf, "{wневидимых вещей"); break;
    }
  }
  strcat (buf, "{x");
  return buf;
}

const char * act_ending (char *buf, int sex, const char *str)
{
  int cnt=0, iPhase;
  const char *tstr = str;
  tstr++;

  if ( sex == 0 ) iPhase=PHASE_COPYEND;
  else iPhase=PHASE_FINDNEED;

  while (*tstr != '\0' && *tstr != ' ' && *tstr != '-')
  {
    if ( *tstr == '|' )
    {
      if ( iPhase == PHASE_COPYEND ) iPhase=PHASE_FINDNEXT;
      if ( iPhase == PHASE_COPYTOBUF ) iPhase=PHASE_FINDNEED;

      if ( iPhase == PHASE_FINDNEED )
      {
        sex--;
        if ( sex <=0 ) iPhase = PHASE_COPYEND;
      }
    }
    else
    if ( iPhase == PHASE_COPYEND  || iPhase == PHASE_COPYTOBUF )
    {
      buf[cnt] = *tstr;
      cnt++;
      buf[cnt] = 0;
    }
    tstr++;
  }
  tstr--;
  return tstr;
}

/* The colour version of the act_new( ) function, -Lope */
void act_new( const char *format, CHAR_DATA *ch, const void *arg1,
              const void *arg2, int type, int min_pos )
{
  static char * const he_she   [] = { "it",  "he",  "she" };
  static char * const him_her  [] = { "it",  "him", "her" };
  static char * const his_her  [] = { "its", "his", "her" };
  static char * const on_ona   [] = { "оно", "он",  "она" };
  static char * const ego_ee   [] = { "его", "его", "ее" };
  static char * const emu_ei   [] = { "ему", "ему", "ей" };
  static char * const sam_sama [] = { "сам", "сам", "сама" };
  static char * const samomu_samoj[] = { "самому", "самому", "самой" };
  static char * const nim_nej  [] = { "ним", "ним", "ней" };
  static char * const nemu_nej [] = { "нему", "нему", "ней" };
  static char * const nemu_nee [] = { "нему", "нему", "неe" };
  static char * const sa_as    [] = { "ся", "ся", "ась" };
  static char * const a_a      [] = { "", "", "a" };
  static char * const ij_aja   [] = { "ый", "ый", "ая" };
  static char * const im_oy    [] = { "ым", "ым", "ой" };

  CHAR_DATA           *to;
  CHAR_DATA           *vch = ( CHAR_DATA * ) arg2;
  OBJ_DATA            *obj1 = ( OBJ_DATA  * ) arg1;
  OBJ_DATA            *obj2 = ( OBJ_DATA  * ) arg2;
  const       char    *str;
  char                temp [MAX_STRING_LENGTH];
  const char                *i = NULL;
  char                *point;
  char                *pbuff;
  char                buffer[ MAX_STRING_LENGTH ];
  char                buf[ MAX_STRING_LENGTH   ];

  /* Discard null and zero-length messages.*/
  if( !format || !*format ) return;

  /* discard null rooms and chars */
  if( !ch || !ch->in_room ) return;

  to = char_list;
  if( type == TO_VICT )
  {
    if( !vch )
    {
      bug( "Act: null vch with TO_VICT.", 0 );
      return;
    }
    if( !vch->in_room ) return;
  }

  for( ; to ; to = to->next )
  {
    if (to->position < min_pos) continue;
    if (IS_NPC(to) && !HAS_TRIGGER(to, TRIG_ACT)) continue;
    if (!IS_NPC(to) && (!to->desc || to->desc->connected!=CON_PLAYING)) continue;

    if (type == TO_CHAR && to != ch) continue;
    if (type == TO_VICT && (to!=vch || to==ch)) continue;
    if (type == TO_ROOM && (to==ch || ch->in_room!=to->in_room || !can_see(to,ch,CHECK_LVL) || !can_see_sneak(to,ch,CHECK_LVL))) continue;
    if (type == TO_NOTVICT && (to==ch || to==vch || ch->in_room != to->in_room || !can_see(to,ch,CHECK_LVL) || !can_see_sneak(to,ch,CHECK_LVL))) continue;
    if (type == TO_ALL && (to==ch || to==vch)) continue;
    if (type == TO_ALL_IN_ROOM && (ch->in_room!=to->in_room || !can_see(to,ch,CHECK_LVL) || !can_see_sneak(to,ch,CHECK_LVL))) continue;
    if (type == TO_NOTCHARVICT && (to != ch) && (to !=vch)) continue;

    point   = buf;
    str     = format;
    while( *str != '\0' )
    {
      if( *str != '$' )
      {
        *point++ = *str++;
        continue;
      }

      ++str;
      i = " <   > ";
      if( !arg2 && *str >= 'A' && *str <= 'Z' )
      {
        bug( "Act: missing arg2 for code %d.", *str );
        i = " <   > ";
      }
      else
      {
        switch (*str)
        {
          default:  bug ( "act_output: unknown code",0);
                    i = "<   >";
                    break;
                    if ( *str=='o' || *str=='O' || *str=='c' || *str=='C' )
                    if ( str[1]< '1' || str[1]> '8' )
                    {
                       bug ( "act_output: unknown option.",0);
                       continue;
                    }

          /* Thx alex for 't' idea */
          case 't': i = (char *) arg1;                            break;
          case 'T': i = (char *) arg2;                            break;
          case 'n': i = PERS( ch,  to  );                         break;
          case 'N': i = PERS( vch, to  );                         break;
          case 'e': i = he_she  [URANGE(0, ch  ->sex, 2)];        break;
          case 'E': i = he_she  [URANGE(0, vch ->sex, 2)];        break;
          case 'm': i = him_her [URANGE(0, ch  ->sex, 2)];        break;
          case 'M': i = him_her [URANGE(0, vch ->sex, 2)];        break;
          case 's': i = his_her [URANGE(0, ch  ->sex, 2)];        break;
          case 'S': i = his_her [URANGE(0, vch ->sex, 2)];        break;
          case 'o': i = on_ona  [URANGE(0, ch  ->sex, 2)];        break;
          case 'O': i = on_ona  [URANGE(0, vch ->sex, 2)];        break;
          case 'g': i = ego_ee  [URANGE(0, ch  ->sex, 2)];        break;
          case 'G': i = ego_ee  [URANGE(0, vch ->sex, 2)];        break;
          case 'u': i = emu_ei  [URANGE(0, ch  ->sex, 2)];        break;
          case 'U': i = emu_ei  [URANGE(0, vch ->sex, 2)];        break;
          case 'j': i = sam_sama  [URANGE(0, ch  ->sex, 2)];        break;
          case 'J': i = sam_sama  [URANGE(0, vch ->sex, 2)];        break;
          case 'l': i = samomu_samoj  [URANGE(0, ch  ->sex, 2)];        break;
          case 'L': i = samomu_samoj  [URANGE(0, vch ->sex, 2)];        break;
          case 'f': i = nim_nej  [URANGE(0, ch  ->sex, 2)];        break;
          case 'F': i = nim_nej  [URANGE(0, vch ->sex, 2)];        break;
          case 'a': i = nemu_nej  [URANGE(0, ch  ->sex, 2)];        break;
          case 'A': i = nemu_nej  [URANGE(0, vch ->sex, 2)];        break;
          case 'q': i = nemu_nee  [URANGE(0, ch  ->sex, 2)];        break;
          case 'Q': i = nemu_nee  [URANGE(0, vch ->sex, 2)];        break;
          case 'r': i = a_a  [URANGE(0, ch  ->sex, 2)];        break;
          case 'R': i = a_a  [URANGE(0, vch ->sex, 2)];        break;
          case 'z': i = sa_as  [URANGE(0, ch  ->sex, 2)];        break;
          case 'Z': i = sa_as  [URANGE(0, vch ->sex, 2)];        break;
          case 'y': i = ij_aja  [URANGE(0, ch  ->sex, 2)];        break;
          case 'Y': i = ij_aja  [URANGE(0, vch ->sex, 2)];        break;
          case 'b': i = im_oy  [URANGE(0,  ch ->sex, 2)];      break;
          case 'B': i = im_oy  [URANGE(0, vch ->sex, 2)];      break;
          /* ... */
          case '$': strcpy (temp, "$"); i = temp; break;
          case 'p': i = act_parse_obj (temp, to, obj1, '1'); break;
          case 'P': i = act_parse_obj (temp, to, obj2, '1'); break;
          case 'd': if ( arg2 == NULL || ((char *) arg2)[0] == '\0' )
                    i = "дверь";
                    else
                    {
                      one_argument( (char *) arg2, temp );
                      i = temp;
                    }
                    break;
          case 'w': str = act_ending (temp, ch->sex, str); i = temp; break;
          case 'W': str = act_ending (temp, vch->sex, str); i = temp; break;
          case 'i': i = act_parse_obj (temp, to, obj1, str[1]); str++; break;
          case 'I': i = act_parse_obj (temp, to, obj2, str[1]); str++; break;
          case 'c': i = act_parse_name (temp, ch, to, str[1]); str++; break;
          case 'C': i = act_parse_name (temp, vch, to, str[1]); str++; break;
        }
      }
      str++;
      if (i)
      {
        while (*i != '\0')
        *point++ = *i++;
      }
    }

    *point++ = '{';
    *point++ = 'x';
    *point++ = '\n';
    *point++ = '\r';
    *point   = '\0';
    buf[0]   = UPPER(buf[0]);
    pbuff    = buffer;
    colourconv( pbuff, buf, to );
    if ( to->desc != NULL ) write_to_buffer( to->desc, buffer, 0 );
    else if ( MOBtrigger ) mp_act_trigger( buf, to, ch, arg1, arg2, TRIG_ACT );
  }
}

/* Windows 95 and Windows NT support functions
 * (copied from Envy) */
#if defined( WIN32 )
void gettimeofday( struct timeval *tp, void *tzp )
{
  tp->tv_sec  = time( NULL );
  tp->tv_usec = 0;
}
#endif

int colour( char type, CHAR_DATA *ch, char *string )
{
  char code[ 20 ];
  char *p = '\0';

  if( IS_NPC( ch ) ) return( 0 );

  switch( type )
  {
    default:
      do_printf( code, CLEAR );
      break;
    case 'x':
      do_printf( code, CLEAR );
      break;
    case 'b':
      do_printf( code, C_BLUE );
      break;
    case 'c':
      do_printf( code, C_CYAN );
      break;
    case 'g':
      do_printf( code, C_GREEN );
      break;
    case 'm':
      do_printf( code, C_MAGENTA );
      break;
    case 'r':
      do_printf( code, C_RED );
      break;
    case 'w':
      do_printf( code, C_WHITE );
      break;
    case 'y':
      do_printf( code, C_YELLOW );
      break;
    case 'B':
      do_printf( code, C_B_BLUE );
      break;
    case 'C':
      do_printf( code, C_B_CYAN );
      break;
    case 'G':
      do_printf( code, C_B_GREEN );
      break;
    case 'M':
      do_printf( code, C_B_MAGENTA );
      break;
    case 'R':
      do_printf( code, C_B_RED );
      break;
    case 'W':
      do_printf( code, C_B_WHITE );
      break;
    case 'Y':
      do_printf( code, C_B_YELLOW );
      break;
    case 'D':
      do_printf( code, C_D_GREY );
      break;
    case '*':
      do_printf( code, "%c", 007 );
      break;
    case '/':
      do_printf( code, "%c", 012 );
      break;
    case '{':
      do_printf( code, "%c", '{' );
      break;
    case '`':
      do_printf( code, "%c", '`' );
      break;
    case '-':
      do_printf( code, "%c", '~' );
      break;
    case '+':
      do_printf( code, C_FLASHON );
      break;
    case '_':
      do_printf( code, C_FLASHOFF );
      break;
  }

  p = code;
  while( *p != '\0' )
  {
    *string = *p++;
    *++string = '\0';
  }

  return( strlen( code ) );
}

void colourconv( char *buffer, const char *txt, CHAR_DATA *ch )
{
  const char *point;
  int   skip = 0;

  if( ch->desc && txt )
  {
    if( IS_SET( ch->act, PLR_COLOUR ) )
    {
      for( point = txt ; *point ; point++ )
      {
        if( *point == '{' || *point == '`' )
        {
          point++;
          skip = colour( *point, ch, buffer );
          while( skip-- > 0 ) ++buffer;
          continue;
        }
        *buffer = *point;
        *++buffer = '\0';
      }
      *buffer = '\0';
    }
    else
    {
      for( point = txt ; *point ; point++ )
      {
        if( *point == '{' || *point == '`' )
        {
          point++;
          continue;
        }
        *buffer = *point;
        *++buffer = '\0';
      }
      *buffer = '\0';
    }
  }
}

void printf_to_char (CHAR_DATA *ch, char *fmt, ...)
{
  char buff [MAX_STRING_LENGTH];
  char * buf;

  char *p;
  char tmp[16];
  int format;
  bool left=FALSE,fmtenable=FALSE,fMatch=FALSE;

  va_list data;
  va_start(data,fmt);

  memset (buff, 0, MAX_STRING_LENGTH) ;
  buf = buff ;

#include "printf.c"

  *buf = 0;
  stc(buff, ch);
}

void write_to_buffer_old( DESCRIPTOR_DATA *d, const char *txt, int length )
{
  if (d==NULL) return;
  if ( length <= 0 ) length = strlen(txt);

  if ( d->outtop == 0 && !d->fcommand )
  {
    d->outbuf[0] = '\n';
    d->outbuf[1] = '\r';
    d->outtop   = 2;
  }
  while ( d->outtop + length >= d->outsize )
  {
    char *outbuf;

    if (d->outsize >= 32000)
    {
      bug("Buffer overflow. Closing.\n\r",0);
      close_socket(d);
      return;
    }
    outbuf      = alloc_mem( 2 * d->outsize );
    strncpy( outbuf, d->outbuf, d->outtop );
    free_mem( d->outbuf, d->outsize );
    d->outbuf   = outbuf;
    d->outsize *= 2;
  }
  strcpy( d->outbuf + d->outtop, txt );
  d->outtop += length;
}

bool personal_ban(const char *addr, const char *hosts)
{
  char host[MAX_STRING_LENGTH];

  while (!EMPTY(hosts))
  {
    hosts=one_argument(hosts,host);
    if (!str_cmp(host,addr)) return TRUE;
  }
  return FALSE;
}
