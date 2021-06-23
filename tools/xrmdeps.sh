#!/bin/bash

FLAVOR=`grep '^ID=' /etc/os-release | awk -F= '{print $2}' | tr -d '"'`
VERSION=`grep '^VERSION_ID=' /etc/os-release | awk -F= '{print $2}' | tr -d '"'`
MAJOR=${VERSION%.*}
ARCH=`uname -m`

usage()
{
    echo "Usage: xrmdeps.sh [options]"
    echo
    echo "[-help]       List this help"
    echo "[-validate]   Validate that required packages are installed"
    echo "[-docker]     Indicate that script is run within a docker container, disables select packages"

    exit 1
}

validate=0
docker=0

while [ $# -gt 0 ]; do
    case "$1" in
        -help)
            usage
            ;;
        -validate)
            validate=1
            shift
            ;;
        -docker)
            docker=1
            shift
            ;;
        *)
            echo "unknown option"
            usage
            ;;
    esac
done

rh_package_list()
{
    RH_LIST=(\
     make \
     cmake \
     gcc \
     gcc-c++ \
     git \
     glibc-static \
     libstdc++-static \
     libuuid-devel \
     boost-system \
     boost-serialization \
     boost-filesystem \
     boost-thread \
     pkgconfig \
     rpm-build \
    )

    if [ $FLAVOR == "amzn" ]; then
        RH_LIST+=(\
        system-lsb-core \
        )
    else
        RH_LIST+=(\
        redhat-lsb \
        )
    fi
}

ub_package_list()
{
    UB_LIST=(\
     make \
     cmake \
     gcc \
     g++ \
     git \
     linux-libc-dev \
     uuid-dev \
     libboost-system-dev \
     libboost-serialization-dev \
     libboost-filesystem-dev \
     libboost-thread-dev \
     pkg-config \
     lsb-release \
     libsystemd-dev \
    )
}

fd_package_list()
{
    FD_LIST=(\
     make \
     cmake \
     gcc \
     gcc-c++ \
     git \
     glibc-static \
     libstdc++-static \
     libuuid-devel \
     boost-system \
     boost-serialization \
     boost-filesystem \
     boost-thread \
     pkgconfig \
     redhat-lsb \
     rpm-build \
    )
}

update_package_list()
{
    if [ $FLAVOR == "ubuntu" ] || [ $FLAVOR == "debian" ]; then
        ub_package_list
    elif [ $FLAVOR == "centos" ] || [ $FLAVOR == "rhel" ] || [ $FLAVOR == "amzn" ]; then
        rh_package_list
    elif [ $FLAVOR == "fedora" ]; then
        fd_package_list
    else
        echo "unknown OS flavor $FLAVOR"
        exit 1
    fi
}

validate()
{
    if [ $FLAVOR == "ubuntu" ] || [ $FLAVOR == "debian" ]; then
        #apt -qq list "${UB_LIST[@]}"
        dpkg -l "${UB_LIST[@]}" > /dev/null
    fi

    if [ $FLAVOR == "centos" ] || [ $FLAVOR == "rhel" ] || [ $FLAVOR == "amzn" ]; then
        rpm -q "${RH_LIST[@]}"
    fi
}

prep_ubuntu()
{
    echo "Preparing ubuntu ..."
}

prep_centos7()
{
    echo "Enabling EPEL repository..."
    rpm -q --quiet epel-release
    if [ $? != 0 ]; then
        yum install -y https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm
        yum check-update
    fi
    echo "Installing cmake3 from EPEL repository..."
    yum install -y cmake3
    if [ $docker == 0 ]; then 
        echo "Enabling CentOS SCL repository..."
        yum --enablerepo=extras install -y centos-release-scl
    fi
}

prep_rhel7()
{
    echo "Enabling EPEL repository..."
    rpm -q --quiet epel-release
    if [ $? != 0 ]; then
        yum install -y https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm
        yum check-update
    fi

    echo "Enabling RHEL SCL repository..."
    yum-config-manager --enable rhel-server-rhscl-7-rpms

    echo "Enabling repository 'rhel-7-server-e4s-optional-rpms"
    subscription-manager repos --enable "rhel-7-server-e4s-optional-rpms"

    echo "Enabling repository 'rhel-7-server-optional-rpms'"
    subscription-manager repos --enable "rhel-7-server-optional-rpms"
}

prep_centos8()
{
    echo "Enabling EPEL repository..."
    rpm -q --quiet epel-release
    if [ $? != 0 ]; then
        yum install -y https://dl.fedoraproject.org/pub/epel/epel-release-latest-8.noarch.rpm
        yum check-update
    fi
    echo "Installing cmake3 from EPEL repository..."
    yum install -y cmake3
    echo "Enabling PowerTools repo for CentOS8 ..."
    yum install -y dnf-plugins-core
    yum config-manager --set-enabled PowerTools
    yum config-manager --set-enabled AppStream
}

prep_rhel8()
{
    echo "Enabling EPEL repository..."
    rpm -q --quiet epel-release
    if [ $? != 0 ]; then
        yum install -y https://dl.fedoraproject.org/pub/epel/epel-release-latest-8.noarch.rpm
     yum check-update
    fi

    echo "Enabling CodeReady-Builder repository..."
    subscription-manager repos --enable "codeready-builder-for-rhel-8-x86_64-rpms"
}

prep_centos()
{
    if [ $MAJOR == 8 ]; then
        prep_centos8
    else
        prep_centos7
    fi
}

prep_rhel()
{
    if [ $MAJOR == 8 ]; then
        prep_rhel8
    else
        prep_rhel7
    fi

    echo "Installing cmake3 from EPEL repository..."
    yum install -y cmake3
}

prep_amzn()
{
    echo "Installing amazon EPEL..."
    amazon-linux-extras install epel
    echo "Installing cmake3 from EPEL repository..."
    yum install -y cmake3
}

install()
{
    if [ $FLAVOR == "ubuntu" ] || [ $FLAVOR == "debian" ]; then
        prep_ubuntu

        echo "Installing packages..."
        apt install -y "${UB_LIST[@]}"
    fi

    # Enable EPEL on CentOS/RHEL/Amazon
    if [ $FLAVOR == "centos" ]; then
        prep_centos
    elif [ $FLAVOR == "rhel" ]; then
        prep_rhel
    elif [ $FLAVOR == "amzn" ]; then
        prep_amzn
    fi

    if [ $FLAVOR == "rhel" ] || [ $FLAVOR == "centos" ] || [ $FLAVOR == "amzn" ]; then
        echo "Installing RHEL/CentOS/Amazon packages..."
        yum install -y "${RH_LIST[@]}"
        if [ $FLAVOR == "rhel" ] || [ $FLAVOR == "centos" ]; then
            if [ $VERSION -lt "8" ]; then
                yum install -y devtoolset-6
            fi
        fi
    fi

    if [ $FLAVOR == "fedora" ]; then
        echo "Installing Fedora packages..."
        yum install -y "${FD_LIST[@]}"
    fi
}

update_package_list

if [ $validate == 1 ]; then
    validate
else
    install
fi
