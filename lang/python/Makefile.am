PYSRC = $(top_srcdir)/lang/python
PY_INCLUDE_DIRS = $(top_srcdir)
all-local: _wiredtiger.so

# We keep generated Python sources under lang/python: that's where they live
# in release packages.
$(PYSRC)/wiredtiger_wrap.c: $(top_srcdir)/src/include/wiredtiger.in $(PYSRC)/wiredtiger.i
	(cd $(PYSRC) && \
	    $(SWIG) -python -threads -O -Wall -nodefaultctor -nodefaultdtor -I$(abs_top_builddir) wiredtiger.i && \
	    mv wiredtiger.py wiredtiger/__init__.py)

_wiredtiger.so: $(top_builddir)/libwiredtiger.la $(PYSRC)/wiredtiger_wrap.c
	$(PYTHON) $(PYSRC)/setup.py build_ext -b . -t . -f -I $(PY_INCLUDE_DIRS)

install-exec-local:
	$(PYTHON) $(PYSRC)/setup.py build_py -d build
	$(PYTHON) $(PYSRC)/setup.py build_ext -b build -t . -f -I $(PY_INCLUDE_DIRS)
	$(PYTHON) $(PYSRC)/setup.py install_lib -b build --skip-build $(PYTHON_INSTALL_ARG)

clean-local:
	$(PYTHON) $(PYSRC)/setup.py clean
	rm -rf _wiredtiger.so WT_TEST build wiredtiger

TESTS = run-ex_access
