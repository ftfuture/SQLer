#include "postgresqlmanager.h"


PostgresqlManager::PostgresqlManager()
{

}

void PostgresqlManager::loadSchema(const QString &t)
{
    schemaModel->setQuery(QString("SELECT attname, pg_catalog.format_type(atttypid, atttypmod),"
                                  " pg_catalog.pg_get_expr(adbin, adrelid), attnotnull, conname"
                                  " FROM pg_catalog.pg_class INNER JOIN pg_catalog.pg_attribute ON attrelid = pg_class.oid"
                                  " LEFT JOIN pg_catalog.pg_attrdef ON adrelid = attrelid AND adnum = attnum AND atthasdef"
                                  " LEFT JOIN pg_catalog.pg_constraint ON conrelid = pg_class.oid AND attnum = ANY(conkey)"
                                  " WHERE pg_class.relname = '%1' AND pg_catalog.pg_table_is_visible(pg_class.oid)"
                                  " AND attnum > 0 AND atttypid <> 0::oid AND NOT attisdropped ORDER BY attnum")
                          .arg(t), database);
}

void PostgresqlManager::loadIndex(const QString &t)
{
    indexModel->setQuery(QString(
                             "SELECT ix.indkey, ix.indoption "
                             "FROM pg_class t, pg_class i, pg_index ix "
                             "WHERE t.oid = ix.indrelid AND i.oid = ix.indexrelid"
                             " AND t.relkind = 'r' AND t.relname = '%1'").arg(t), database);
}
