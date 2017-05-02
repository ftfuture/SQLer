TEMPLATE = subdirs

CONFIG += ordered
SUBDIRS += \
    DBManager \
    guiSQLer \

guiSQLer.depends = DBManager

