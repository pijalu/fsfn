# Copyright 1999-2005 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2

DESCRIPTION="Sony Vaio (FS series) laptop hotkey handler"
HOMEPAGE="http://developer.berlios.de/projects/fsfn/"
SRC_URI="http://download.berlios.de/fsfn/${P}.tar.gz"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~x86"
IUSE="xosd"

DEPEND="
	xosd? ( >=x11-libs/xosd-2.2.0 )
	>=app-laptop/sonyacpi-0.1
	dev-lang/perl
"

src_unpack() {
	unpack ${A}
	cd ${S}
	
	# fix dir for bug 5951
	sed -i \
		-e "s:/usr/local/bin:/usr/bin:g" \
		${S}/gentoo/fsfn
}

src_compile () {
	use xosd || my_conf="${my_conf} --disable-xosd"

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
	cd ${S}/gentoo
	exeinto /etc/init.d/
	newexe ${S}/gentoo/fsfn fsfn || die
	
	einfo
	einfo "You can use /etc/init.d/fsfn script to start fsfn"
	einfo
}
