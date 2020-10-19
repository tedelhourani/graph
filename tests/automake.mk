# The `:;' works around a Bash 3.2 bug when the output is not writeable.
$(srcdir)/package.m4: $(top_srcdir)/configure.ac
        $(AM_V_GEN):;{ \
          echo '# Signature of the current package.' && \
          echo 'm4_define([AT_PACKAGE_NAME],      [$(PACKAGE_NAME)])' && \
          echo 'm4_define([AT_PACKAGE_TARNAME],   [$(PACKAGE_TARNAME)])' && \
          echo 'm4_define([AT_PACKAGE_VERSION],   [$(PACKAGE_VERSION)])' && \
          echo 'm4_define([AT_PACKAGE_STRING],    [$(PACKAGE_STRING)])' && \
          echo 'm4_define([AT_PACKAGE_BUGREPORT], [$(PACKAGE_BUGREPORT)])'; \
        } >'$(srcdir)/package.m4'

EXTRA_DIST = tests/testsuite.at $(srcdir)/package.m4 $(TESTSUITE) atlocal.in
TESTSUITE = $(srcdir)/testsuite
AUTOTEST_PATH = tests

atconfig: $(top_builddir)/config.status
	cd $(top_builddir) && $(SHELL) ./config.status tests/$@

atlocal: tests/atlocal.in $(top_builddir)/config.status
	cd $(top_builddir) && $(SHELL) ./config.status tests/$@

check-local: atconfig atlocal $(TESTSUITE)
	$(SHELL) '$(TESTSUITE)' AUTOTEST_PATH=$(AUTOTEST_PATH)
	$(TESTSUITEFLAGS)

installcheck-local: atconfig atlocal $(TESTSUITE)
	$(SHELL) '$(TESTSUITE)' AUTOTEST_PATH=$(AUTOTEST_PATH)
	$(TESTSUITEFLAGS)

clean-local:
	test ! -f '$(TESTSUITE)' || \
	$(SHELL) '$(TESTSUITE)' --clean

AUTOTEST = $(AUTOM4TE) --language=autotest
$(TESTSUITE): tests/testsuite.at $(srcdir)/package.m4
	$(AM_V_GEN)$(AUTOTEST) -I '$(srcdir)' -o $@.tmp tests/$@.at
	mv $@.tmp $@

tests_alg_SOURCES = tests/alg.c
tests_alg_HEADERS = 
noinst_PROGRAMS = tests/alg
tests_test_lib_LDADD = lib/libgraph.la
