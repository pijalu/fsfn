# Copyright 1999-2005 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

inherit eutils linux-mod

DESCRIPTION="Linux kernel modules for Sony ACPI special items"
HOMEPAGE="http://developer.berlios.de/projects/fsfn/"
SRC_URI="http://download.berlios.de/fsfn/${P}.tar.gz"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~x86"
IUSE=""

DEPEND="virtual/linux-sources"
RDEPEND="virtual/modutils"

BUILD_TARGETS="modules"
MODULE_NAMES="sony_acpi(kernel/drivers/acpi:)"

CONFIG_CHECK="ACPI"
ERROR_ACPI="${P} requires ACPI to be enabled for kernel (CONFIG_ACPI)"
	
pkg_setup() {
	linux-mod_pkg_setup

	if kernel_is 2 4; then
		die "${P} does not support building against kernel 2.4.x"
	fi

	BUILD_PARAMS="KSRC=${KV_DIR} KSRC_OUTPUT=${KV_OUT_DIR}"
}

src_unpack() {
	S=${WORKDIR}/${P}
	unpack ${A}
	cd ${S}
}

src_compile() {
	linux-mod_src_compile
}


src_install() {
	linux-mod_src_install
}

pkg_postinst() {
	linux-mod_pkg_postinst
	einfo
	einfo "You can now add sony_acpi to your modules.autoload list"
	einfo "echo sony_acpi >> /etc/modules.autoload.d/kernel-2.6"
	einfo
}


