# Copyright 1999-2005 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2

DESCRIPTION="Sony Vaio (FS series) laptop hotkey handler"
HOMEPAGE="http://developer.berlios.de/projects/fsfn/"
SRC_URI="http://users.skynet.be/muaddib/fsfn/${P}.tar.gz"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~x86"
IUSE="xosd moreconf"

DEPEND="
	xosd? ( >=x11-libs/xosd-2.2.0 )
	dev-lang/perl
"

src_unpack() {
	unpack ${A}
	cd ${S}	
}

src_compile () {
	use xosd || my_conf="${my_conf} --disable-xosd"
	use moreconf && my_conf="${my_conf} --enable-moreconf"

	# just give a few info
	if use xosd; then
		einfo
		einfo "xosd support built in"
		einfo "To enable it, please add 'fsfn -o' to your x session"
		einfo
	fi

	econf ${my_conf} || die "configuration failed"

	emake || die "make of fsfn failed"
}

src_install() {
	make DESTDIR="${D}" install || die "Installation failed!"
	doinitd ${S}/gentoo/fsfn || die "Failed to install initd script"
	newconfd ${S}/gentoo/fsfn-conf fsfn || die "Failed to install config script"	
	einfo
	einfo "You can use /etc/init.d/fsfn script to start fsfn"
	einfo
}
