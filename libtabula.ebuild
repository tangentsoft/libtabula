# Copyright 1999-2008 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: /var/cvsroot/gentoo-x86/dev-db/libtabula/libtabula-2.3.2.ebuild,v 1.3 2008/04/21 03:00:49 dirtyepic Exp $

inherit eutils

DESCRIPTION="C++ wrapper for multiple C DBMS APIs"
HOMEPAGE="http://libtabula.org/"
SRC_URI="http://libtabula.org/releases/${P}.tar.gz"

LICENSE="LGPL-2"
SLOT="0"
KEYWORDS="~alpha ~amd64 ~hppa ~mips ~ppc ~sparc ~x86"
IUSE=""

DEPEND=">=sys-devel/gcc-3"
RDEPEND="${DEPEND}
		>=virtual/mysql-4.0"

src_unpack() {
	unpack ${A}
	cd "${S}"

	epatch "${FILESDIR}"/${P}-gcc-4.3.patch

	for i in "${S}"/lib/*.h ; do
		sed -i \
			-e '/#include </s,mysql.h,mysql/mysql.h,g' \
			-e '/#include </s,mysql_version.h,mysql/mysql_version.h,g' \
			"${i}" || die "Failed to sed ${i} for fixing MySQL includes"
	done
}

src_compile() {
	local myconf
	# we want C++ exceptions turned on
	myconf="--enable-exceptions"
	# give threads a try
	myconf="${myconf} --enable-thread-check"
	# not including the directives to where MySQL is because it seems to
	# find it just fine without

	# force the cflags into place otherwise they get totally ignored by
	# configure
	CFLAGS="${CFLAGS}" CXXFLAGS="${CXXFLAGS}" \
	econf ${myconf} || die "econf failed"

	emake || die "unable to make"
}

src_install() {
	emake DESTDIR="${D}" install || die
	# install the docs and HTML pages
	dodoc README* CREDITS.md ChangeLog.md CONTRIBUTING.md Wishlist.md
	dodoc doc/*
	cp -ra doc/html "${D}"/usr/share/doc/${PF}/html
	prepalldocs
}
