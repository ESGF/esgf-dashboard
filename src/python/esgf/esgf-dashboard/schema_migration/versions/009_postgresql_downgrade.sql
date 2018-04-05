SET search_path = esgf_dashboard, pg_catalog;

/* CROSS DW DROP TABLES */
DROP TABLE IF EXISTS esgf_dashboard.cross_dmart_project_host CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cross_fact_download CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cross_dim_date CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cross_dim_geolocation CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cross_bridge_project CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cross_dim_project CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cross_dmart_project_host_time CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cross_dmart_project_host_geolocation CASCADE;

/* OBS4MIPS DROP TABLES */
DROP TABLE IF EXISTS esgf_dashboard.obs4mips_dim_geolocation CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.obs4mips_dim_date CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.obs4mips_dim_dataset CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.obs4mips_dim_file CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.obs4mips_dim_institute CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.obs4mips_bridge_institute CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.obs4mips_dim_variable CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.obs4mips_bridge_variable CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.obs4mips_dim_time_frequency CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.obs4mips_bridge_time_frequency CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.obs4mips_dim_processing_level CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.obs4mips_bridge_processing_level CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.obs4mips_dim_source_id CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.obs4mips_bridge_source_id CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.obs4mips_dim_realm CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.obs4mips_bridge_realm CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.obs4mips_fact_download CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.obs4mips_dmart_clients_time_geolocation CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.obs4mips_dmart_variable_host_time CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.obs4mips_dmart_source_host_time CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.obs4mips_dmart_realm_host_time CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.obs4mips_dmart_dataset_host_time CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.obs4mips_dmart_clients_host_geolocation CASCADE;

/* CMIP5 DROP TABLES */
DROP TABLE IF EXISTS esgf_dashboard.cmip5_dim_geolocation CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip5_dim_date CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip5_dim_dataset CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip5_dim_time_frequency CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip5_bridge_time_frequency CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip5_dim_variable CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip5_bridge_variable CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip5_dim_experiment CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip5_bridge_experiment CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip5_dim_model CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip5_bridge_model CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip5_dim_realm CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip5_bridge_realm CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip5_dim_institute CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip5_bridge_institute CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip5_fact_download CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip5_dmart_clients_time_geolocation CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip5_dmart_model_host_time CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip5_dmart_experiment_host_time CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip5_dmart_variable_host_time CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip5_dmart_dataset_host_time CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip5_dmart_clients_host_geolocation CASCADE;

/* REGISTRY DROP TABLE */
DROP TABLE IF EXISTS esgf_dashboard.registry CASCADE;
SET search_path = public, pg_catalog;

