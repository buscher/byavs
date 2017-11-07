TEMPLATE = subdirs 
CONFIG += warn_on \
          qt \
          thread
SUBDIRS += src

unix {
    SUBDIRS += generate_level01
}

