#!/bin/csh -f

#set called=($_)
#set script_path=""
set xrm_dir="/opt/xilinx/xrm"

# revisit if there is a better way than lsof to obtain the script path
# in non-interactive mode.  If lsof is needed, then revisit why
# why sbin need to be prepended looks like some environment issue in
# user shell, e.g. /usr/local/bin/mis_env: No such file or directory.
# is because user path contain bad directories that are searched when
# looking of lsof.
set path=(/usr/sbin $path)
set called=(`\lsof +p $$ |\grep setup.csh`)

set OSDIST=`lsb_release -i |awk -F: '{print tolower($2)}' | tr -d ' \t'`
set OSREL=`lsb_release -r |awk -F: '{print tolower($2)}' |tr -d ' \t' | awk -F. '{print $1*100+$2}'`

if ( "$OSDIST" =~ "ubuntu" ) then
    if ( $OSREL < 1604 ) then
        echo "ERROR: Ubuntu release version must be 16.04 or later"
        exit 1
    endif
endif

if ( "$OSDIST" =~ centos  || "$OSDIST" =~ redhat* ) then
    if ( $OSREL < 704 ) then
        echo "ERROR: Centos or RHEL release version must be 7.4 or later"
        exit 1
    endif
endif

setenv XILINX_XRM $xrm_dir

if ( ! $?LD_LIBRARY_PATH ) then
   setenv LD_LIBRARY_PATH $XILINX_XRM/lib
else
   setenv LD_LIBRARY_PATH $XILINX_XRM/lib:$LD_LIBRARY_PATH
endif

if ( ! $?PATH ) then
   setenv PATH $XILINX_XRM/bin
else
   setenv PATH $XILINX_XRM/bin:$PATH
endif

echo "XILINX_XRM      : $XILINX_XRM"
echo "PATH            : $PATH"
echo "LD_LIBRARY_PATH : $LD_LIBRARY_PATH"
