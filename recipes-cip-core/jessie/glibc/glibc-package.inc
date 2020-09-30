#
# base recipe: meta/recipes-core/eglibc/eglibc-package.inc
# base branch: daisy
#

#
# For now, we will skip building of a gcc package if it is a uclibc one
# and our build is not a uclibc one, and we skip a glibc one if our build
# is a uclibc build.
#
# See the note in gcc/gcc_3.4.0.oe
#

python __anonymous () {
    import bb, re
    uc_os = (re.match('.*uclibc*', d.getVar('TARGET_OS', True)) != None)
    if uc_os:
        raise bb.parse.SkipPackage("incompatible with target %s" %
                                   d.getVar('TARGET_OS', True))
}

INHIBIT_SYSROOT_STRIP = "1"

PACKAGES = "${PN}-dbg catchsegv sln nscd ldd ${PN}-utils glibc-thread-db ${PN}-pic libcidn libmemusage libsegfault ${PN}-pcprofile libsotruss ${PN} glibc-extra-nss ${PN}-dev ${PN}-staticdev ${PN}-doc"

# The ld.so in this glibc supports the GNU_HASH
RPROVIDES_${PN} = "eglibc rtld(GNU_HASH)"
RPROVIDES_${PN}-utils = "eglibc-utils"
RPROVIDES_${PN}-mtrace = "eglibc-mtrace libc-mtrace"
RPROVIDES_${PN}-pic = "eglibc-pic"
RPROVIDES_${PN}-dev = "eglibc-dev libc6-dev virtual-libc-dev"
RPROVIDES_${PN}-staticdev = "eglibc-staticdev"
RPROVIDES_${PN}-doc = "eglibc-doc"
RPROVIDES_glibc-extra-nss = "eglibc-extra-nss"
RPROVIDES_glibc-thread-db = "eglibc-thread-db"
RPROVIDES_${PN}-pcprofile = "eglibc-pcprofile"
RPROVIDES_${PN}-dbg = "eglibc-dbg"
libc_baselibs = "${base_libdir}/libcrypt*.so.* ${base_libdir}/libcrypt-*.so ${base_libdir}/libc.so.* ${base_libdir}/libc-*.so ${base_libdir}/libm*.so.* ${base_libdir}/libm-*.so ${base_libdir}/ld*.so.* ${base_libdir}/ld-*.so ${base_libdir}/libpthread*.so.* ${base_libdir}/libpthread-*.so ${base_libdir}/libresolv*.so.* ${base_libdir}/libresolv-*.so ${base_libdir}/librt*.so.* ${base_libdir}/librt-*.so ${base_libdir}/libutil*.so.* ${base_libdir}/libutil-*.so ${base_libdir}/libnsl*.so.* ${base_libdir}/libnsl-*.so ${base_libdir}/libnss_files*.so.* ${base_libdir}/libnss_files-*.so ${base_libdir}/libnss_compat*.so.* ${base_libdir}/libnss_compat-*.so ${base_libdir}/libnss_dns*.so.* ${base_libdir}/libnss_dns-*.so ${base_libdir}/libdl*.so.* ${base_libdir}/libdl-*.so ${base_libdir}/libanl*.so.* ${base_libdir}/libanl-*.so ${base_libdir}/libBrokenLocale*.so.* ${base_libdir}/libBrokenLocale-*.so"
libc_baselibs_append_aarch64 = " /lib/ld-linux-aarch64*.so.1"
INSANE_SKIP_${PN}_append_aarch64 = " libdir"

FILES_${PN} = "${libc_baselibs} ${libexecdir}/* ${@bb.utils.contains('DISTRO_FEATURES', 'ldconfig', '${base_sbindir}/ldconfig ${sysconfdir}/ld.so.conf', '', d)}"
FILES_ldd = "${bindir}/ldd"
FILES_libsegfault = "${base_libdir}/libSegFault*"
FILES_libcidn = "${base_libdir}/libcidn-*.so ${base_libdir}/libcidn.so.*"
FILES_libmemusage = "${base_libdir}/libmemusage.so"
FILES_glibc-extra-nss = "${base_libdir}/libnss_*-*.so ${base_libdir}/libnss_*.so.*"
FILES_sln = "${base_sbindir}/sln"
FILES_${PN}-pic = "${libdir}/*_pic.a ${libdir}/*_pic.map ${libdir}/libc_pic/*.o"
FILES_libsotruss = "${libdir}/audit/sotruss-lib.so"
FILES_SOLIBSDEV = "${libdir}/lib*${SOLIBSDEV}"
FILES_${PN}-dev += "${bindir}/rpcgen ${libdir}/*_nonshared.a ${base_libdir}/*_nonshared.a ${base_libdir}/*.o ${datadir}/aclocal"
FILES_${PN}-staticdev += "${libdir}/*.a ${base_libdir}/*.a"
FILES_nscd = "${sbindir}/nscd*"
FILES_${PN}-mtrace = "${bindir}/mtrace"
FILES_${PN}-utils = "${bindir}/* ${sbindir}/*"
FILES_${PN}-dbg += "${libexecdir}/*/.debug ${libdir}/audit/.debug"
FILES_catchsegv = "${bindir}/catchsegv"
RDEPENDS_catchsegv = "libsegfault"
FILES_${PN}-pcprofile = "${base_libdir}/libpcprofile.so"
FILES_glibc-thread-db = "${base_libdir}/libthread_db.so.* ${base_libdir}/libthread_db-*.so"
RPROVIDES_${PN}-dev += "libc-dev"
RPROVIDES_${PN}-staticdev += "libc-staticdev"

SUMMARY_sln = "The static ln"
DESCRIPTION_sln = "Similar to the 'ln' utility, but statically linked.  sln is useful to make symbolic links to dynamic libraries if the dynamic linking system, for some reason, is not functional."
SUMMARY_nscd = "Name service cache daemon"
DESCRIPTION_nscd = "nscd, name service cache daemon, caches name service lookups for the passwd, group and hosts information.  It can damatically improvide performance with remote, such as NIS or NIS+, name services."
SUMMARY_glibc-extra-nss = "hesiod, NIS and NIS+ nss libraries"
DESCRIPTION_glibc-extra-nss = "glibc: nis, nisplus and hesiod search services."
SUMMARY_ldd = "print shared library dependencies"
DESCRIPTION_ldd = "${bindir}/ldd prints shared library dependencies for each program or shared library specified on the command line."
SUMMARY_${PN}-utils = "Miscellaneous utilities provided by glibc"
DESCRIPTION_${PN}-utils = "Miscellaneous utilities including getconf, iconf, locale, gencat, tzselect, zic, rpcinfo, ..."
DESCRIPTION_libsotruss = "Library to support sotruss which traces calls through PLTs"

inherit libc-common multilib_header

do_install_append () {
	rm -f ${D}${sysconfdir}/localtime
	rm -rf ${D}${localstatedir}

	# remove empty glibc dir
	if [ -d ${D}${libexecdir} ]; then
		rmdir --ignore-fail-on-non-empty ${D}${libexecdir}
	fi

	oe_multilib_header bits/syscall.h

	if [ -f ${D}${bindir}/mtrace ]; then
		sed -i -e '1s,#!.*perl,#! ${USRBINPATH}/env perl,' -e '2s,exec.*perl,exec ${USRBINPATH}/env perl,' ${D}${bindir}/mtrace
	fi
	rm -rf ${D}${includedir}/rpcsvc/rquota*
	# Info dir listing isn't interesting at this point so remove it if it exists.
	if [ -e "${D}${infodir}/dir" ]; then
		rm -f ${D}${infodir}/dir
	fi

	if ! ${@bb.utils.contains('DISTRO_FEATURES', 'ldconfig', 'true', 'false', d)}; then
		# We won't ship this file (see FILES above) so let's not install it
		rm -f ${D}${sysconfdir}/ld.so.conf
		rm -f ${D}${base_sbindir}/ldconfig
		# This directory will be empty now so remove it too.
		# But check whether it exists first, since it won't for glibc-initial.
		if [ -d ${D}${sysconfdir} ]; then
			rmdir ${D}${sysconfdir}
		fi
	fi
}

do_install_append_aarch64 () {
        if [ "${base_libdir}" != "${nonarch_base_libdir}" ]; then
                # The aarch64 ABI says the dynamic linker -must- be /lib/ld-linux-aarch64[_be].so.1
                install -d ${D}${nonarch_base_libdir}
                if [ -e ${D}${base_libdir}/ld-linux-aarch64.so.1 ]; then
                        ln -s ${@base_path_relative('${nonarch_base_libdir}', '${base_libdir}')}/ld-linux-aarch64.so.1 \
                                ${D}${nonarch_base_libdir}/ld-linux-aarch64.so.1
                elif [ -e ${D}${base_libdir}/ld-linux-aarch64_be.so.1 ]; then
                        ln -s ${@base_path_relative('${nonarch_base_libdir}', '${base_libdir}')}/ld-linux-aarch64_be.so.1 \
                                ${D}${nonarch_base_libdir}/ld-linux-aarch64_be.so.1
                fi
        fi
        do_install_armmultilib
}

do_install_append_arm () {
        do_install_armmultilib
}

do_install_append_armeb () {
        do_install_armmultilib
}

do_install_armmultilib () {

        oe_multilib_header bits/endian.h bits/fcntl.h bits/fenv.h bits/hwcap.h bits/ipc.h bits/mathdef.h bits/link.h
        oe_multilib_header bits/wordsize.h bits/local_lim.h bits/mman.h bits/msq.h bits/pthreadtypes.h bits/sem.h bits/semaphore.h
        oe_multilib_header bits/setjmp.h bits/shm.h bits/sigstack.h bits/stat.h bits/statfs.h bits/typesizes.h

        oe_multilib_header fpu_control.h gnu/lib-names.h gnu/stubs.h ieee754.h

        oe_multilib_header sys/elf.h sys/procfs.h sys/ptrace.h sys/ucontext.h sys/user.h
}

do_install_locale () {
	dest=${D}/${includedir}/glibc-locale-internal-${MULTIMACH_TARGET_SYS}
	install -d ${dest}${base_libdir} ${dest}${bindir} ${dest}${libdir} ${dest}${datadir}
	if [ "${base_libdir}" != "${libdir}" ]; then
		cp -fpPR ${D}${base_libdir}/* ${dest}${base_libdir}
	fi
	if [ -e ${D}${bindir}/localedef ]; then
		mv -f ${D}${bindir}/localedef ${dest}${bindir}
	fi
	if [ -e ${D}${libdir}/gconv ]; then
		mv -f ${D}${libdir}/gconv ${dest}${libdir}
	fi
	if [ -e ${D}${exec_prefix}/lib ]; then
		cp -fpPR ${D}${exec_prefix}/lib ${dest}${exec_prefix}
	fi
	if [ -e ${D}${datadir}/i18n ]; then
		mv ${D}${datadir}/i18n ${dest}${datadir}
	fi
	cp -fpPR ${D}${datadir}/* ${dest}${datadir}
	rm -rf ${D}${datadir}/locale/
	cp -fpPR ${WORKDIR}/SUPPORTED ${dest}
}

addtask do_install_locale after do_install before do_populate_sysroot do_package

bashscripts = "mtrace sotruss xtrace"

do_evacuate_scripts () {
	target=${D}${includedir}/glibc-scripts-internal-${MULTIMACH_TARGET_SYS}
	mkdir -p $target
	for i in ${bashscripts}; do
		if [ -f ${D}${bindir}/$i ]; then
			cp ${D}${bindir}/$i $target/
		fi
	done
}

addtask evacuate_scripts after do_install before do_populate_sysroot do_package

PACKAGE_PREPROCESS_FUNCS += "glibc_package_preprocess"

glibc_package_preprocess () {
	rm -rf ${PKGD}/${includedir}/glibc-locale-internal-${MULTIMACH_TARGET_SYS}
	rm -rf ${PKGD}/${includedir}/glibc-scripts-internal-${MULTIMACH_TARGET_SYS}
	for i in ${bashscripts}; do
	    rm -f ${PKGD}${bindir}/$i
	done
	rm -rf ${PKGD}/${localedir}
	if [ "${libdir}" != "${exec_prefix}/lib" ]; then
		# This dir only exists to hold locales
		rm -rf ${PKGD}${exec_prefix}/lib
	fi
}