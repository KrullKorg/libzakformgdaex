/*
 * Copyright (C) 2015 Andrea Zagli <azagli@libero.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include <libzakform/libzakform.h>

#include "form.h"

static void zak_form_gdaex_provider_class_init (ZakFormGdaexProviderClass *class);
static void zak_form_gdaex_provider_init (ZakFormGdaexProvider *zak_form_gdaex_provider);
static void zak_form_iprovider_interface_init (ZakFormIProviderInterface *iface);

static void zak_form_gdaex_provider_set_property (GObject *object,
                               guint property_id,
                               const GValue *value,
                               GParamSpec *pspec);
static void zak_form_gdaex_provider_get_property (GObject *object,
                               guint property_id,
                               GValue *value,
                               GParamSpec *pspec);

static void zak_form_gdaex_provider_dispose (GObject *gobject);
static void zak_form_gdaex_provider_finalize (GObject *gobject);

static gboolean zak_form_gdaex_provider_load (ZakFormIProvider *provider, GPtrArray *elements);
static gboolean zak_form_gdaex_provider_insert (ZakFormIProvider *provider, GPtrArray *elements);
static gboolean zak_form_gdaex_provider_update (ZakFormIProvider *provider, GPtrArray *elements);
static gboolean zak_form_gdaex_provider_delete (ZakFormIProvider *provider, GPtrArray *elements);

struct _ZakFormGdaexProvider
{
	GObject parent_instance;
};

#define ZAK_FORM_GDAEX_PROVIDER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), ZAK_FORM_GDAEX_TYPE_PROVIDER, ZakFormGdaexProviderPrivate))

typedef struct _ZakFormGdaexProviderPrivate ZakFormGdaexProviderPrivate;
struct _ZakFormGdaexProviderPrivate
	{
		GdaEx *gdaex;
		gchar *table;
	};

G_DEFINE_TYPE_WITH_CODE (ZakFormGdaexProvider, zak_form_gdaex_provider, G_TYPE_OBJECT,
						 G_IMPLEMENT_INTERFACE (ZAK_FORM_TYPE_IPROVIDER,
												zak_form_iprovider_interface_init))

static void
zak_form_gdaex_provider_class_init (ZakFormGdaexProviderClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);

	object_class->set_property = zak_form_gdaex_provider_set_property;
	object_class->get_property = zak_form_gdaex_provider_get_property;
	object_class->dispose = zak_form_gdaex_provider_dispose;
	object_class->finalize = zak_form_gdaex_provider_finalize;

	g_type_class_add_private (object_class, sizeof (ZakFormGdaexProviderPrivate));
}

static void
zak_form_gdaex_provider_init (ZakFormGdaexProvider *zak_form_gdaex_provider)
{
	ZakFormGdaexProviderPrivate *priv = ZAK_FORM_GDAEX_PROVIDER_GET_PRIVATE (zak_form_gdaex_provider);

	priv->gdaex = NULL;
	priv->table = NULL;
}

static void
zak_form_iprovider_interface_init (ZakFormIProviderInterface *iface)
{
	iface->load = zak_form_gdaex_provider_load;
	iface->insert = zak_form_gdaex_provider_insert;
	iface->update = zak_form_gdaex_provider_update;
	iface->delete = zak_form_gdaex_provider_delete;
}

/**
 * zak_form_gdaex_provider_new:
 * @gdaex:
 * @table:
 *
 * Returns: the newly created #ZakFormGdaexProvider object.
 */
ZakFormGdaexProvider
*zak_form_gdaex_provider_new (GdaEx *gdaex, const gchar *table)
{
	ZakFormGdaexProvider *zak_form_gdaex_provider;
	ZakFormGdaexProviderPrivate *priv;

	zak_form_gdaex_provider = ZAK_FORM_GDAEX_PROVIDER (g_object_new (zak_form_gdaex_provider_get_type (), NULL));

	priv = ZAK_FORM_GDAEX_PROVIDER_GET_PRIVATE (zak_form_gdaex_provider);

	priv->gdaex = gdaex;
	priv->table = g_strdup (table);

	return zak_form_gdaex_provider;
}

/* PRIVATE */
static void
zak_form_gdaex_provider_set_property (GObject *object,
                   guint property_id,
                   const GValue *value,
                   GParamSpec *pspec)
{
	ZakFormGdaexProvider *zak_form_gdaex_provider = (ZakFormGdaexProvider *)object;
	ZakFormGdaexProviderPrivate *priv = ZAK_FORM_GDAEX_PROVIDER_GET_PRIVATE (zak_form_gdaex_provider);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
zak_form_gdaex_provider_get_property (GObject *object,
                   guint property_id,
                   GValue *value,
                   GParamSpec *pspec)
{
	ZakFormGdaexProvider *zak_form_gdaex_provider = (ZakFormGdaexProvider *)object;
	ZakFormGdaexProviderPrivate *priv = ZAK_FORM_GDAEX_PROVIDER_GET_PRIVATE (zak_form_gdaex_provider);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
zak_form_gdaex_provider_dispose (GObject *gobject)
{
	ZakFormGdaexProvider *zak_form_gdaex_provider = (ZakFormGdaexProvider *)gobject;
	ZakFormGdaexProviderPrivate *priv = ZAK_FORM_GDAEX_PROVIDER_GET_PRIVATE (zak_form_gdaex_provider);


	GObjectClass *parent_class = g_type_class_peek_parent (G_OBJECT_GET_CLASS (gobject));
	parent_class->dispose (gobject);
}

static void
zak_form_gdaex_provider_finalize (GObject *gobject)
{
	ZakFormGdaexProvider *zak_form_gdaex_provider = (ZakFormGdaexProvider *)gobject;
	ZakFormGdaexProviderPrivate *priv = ZAK_FORM_GDAEX_PROVIDER_GET_PRIVATE (zak_form_gdaex_provider);


	GObjectClass *parent_class = g_type_class_peek_parent (G_OBJECT_GET_CLASS (gobject));
	parent_class->finalize (gobject);
}

static GValue
*zak_form_gdaex_provider_new_gvalue_from_str_type (const gchar *type, const gchar *value)
{
	GValue *ret;

	ret = g_new0 (GValue, 1);
	if (g_ascii_strcasecmp (type, "integer") == 0)
		{
			ret = g_value_init (ret, G_TYPE_INT);
			g_value_set_int (ret, strtol (value, NULL, 10));
		}
	else if (g_ascii_strcasecmp (type, "float") == 0)
		{
			ret = g_value_init (ret, G_TYPE_FLOAT);
			g_value_set_float (ret, g_strtod (value, NULL));
		}
	else if (g_ascii_strcasecmp (type, "string") == 0)
		{
			ret = g_value_init (ret, G_TYPE_STRING);
			g_value_set_string (ret, value);
		}
	else if (g_ascii_strcasecmp (type, "boolean") == 0)
		{
			ret = g_value_init (ret, G_TYPE_BOOLEAN);
			g_value_set_boolean (ret, g_strcmp0 (value, "TRUE") == 0);
		}

	return ret;
}

static gboolean
zak_form_gdaex_provider_load (ZakFormIProvider *provider, GPtrArray *elements)
{
	gboolean ret;

	guint i;

	GdaExSqlBuilder *sqlb;
	gboolean with_key;

	GdaDataModel *dm;

	ZakFormGdaexProviderPrivate *priv = ZAK_FORM_GDAEX_PROVIDER_GET_PRIVATE (provider);

	g_return_val_if_fail (IS_GDAEX (priv->gdaex), FALSE);

	ret = TRUE;

	sqlb = gdaex_sql_builder_new (GDA_SQL_STATEMENT_SELECT);

	gdaex_sql_builder_from (sqlb, priv->table, NULL);

	/* first round to make sql statement */
	with_key = FALSE;
	for (i = 0; i < elements->len; i++)
		{
			ZakFormElement *element = (ZakFormElement *)g_ptr_array_index (elements, i);
			if (zak_form_element_get_to_load (element))
				{
					gdaex_sql_builder_field (sqlb,
											 priv->table,
											 zak_form_element_get_name (element),
											 "",
											 NULL);

				}

			if (zak_form_element_get_is_key (element))
				{
					gdaex_sql_builder_where (sqlb, (with_key ? GDA_SQL_OPERATOR_TYPE_AND : 0),
											 priv->table,
											 zak_form_element_get_name (element),
											 "",
											 GDA_SQL_OPERATOR_TYPE_EQ,
											 zak_form_gdaex_provider_new_gvalue_from_str_type (zak_form_element_get_provider_type (element), zak_form_element_get_value (element)),
											 NULL);
					with_key = TRUE;
				}
		}

	dm = gdaex_sql_builder_query (sqlb, priv->gdaex, NULL);
	if (!gdaex_data_model_is_empty (dm))
		{
			/* second round to fill ZakFormElements */
			for (i = 0; i < elements->len; i++)
				{
					ZakFormElement *element = (ZakFormElement *)g_ptr_array_index (elements, i);
					if (zak_form_element_get_to_load (element))
						{
							zak_form_element_set_value (element,
														gdaex_data_model_get_field_value_stringify_at (dm, 0, zak_form_element_get_name (element)));
							zak_form_element_set_as_original_value (element);
						}
				}
		}
	else
		{
			ret = FALSE;
		}

	return ret;
}

static gboolean
zak_form_gdaex_provider_insert (ZakFormIProvider *provider, GPtrArray *elements)
{
	gboolean ret;

	guint i;

	GdaExSqlBuilder *sqlb;

	GdaDataModel *dm;

	ZakFormGdaexProviderPrivate *priv = ZAK_FORM_GDAEX_PROVIDER_GET_PRIVATE (provider);

	g_return_val_if_fail (IS_GDAEX (priv->gdaex), FALSE);

	ret = TRUE;

	sqlb = gdaex_sql_builder_new (GDA_SQL_STATEMENT_INSERT);

	gdaex_sql_builder_from (sqlb, priv->table, NULL);

	for (i = 0; i < elements->len; i++)
		{
			ZakFormElement *element = (ZakFormElement *)g_ptr_array_index (elements, i);
			if (zak_form_element_get_to_save (element))
				{
					gdaex_sql_builder_field (sqlb,
											 priv->table,
											 zak_form_element_get_name (element),
											 "",
											 zak_form_gdaex_provider_new_gvalue_from_str_type (zak_form_element_get_provider_type (element), zak_form_element_get_value (element)));
				}
		}


	if (gdaex_sql_builder_execute (sqlb, priv->gdaex, NULL) <= 0)
		{
			ret = FALSE;
		}

	return ret;
}

static gboolean
zak_form_gdaex_provider_update (ZakFormIProvider *provider, GPtrArray *elements)
{

}

static gboolean
zak_form_gdaex_provider_delete (ZakFormIProvider *provider, GPtrArray *elements)
{

}
