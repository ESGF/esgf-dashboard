--
-- PostgreSQL updates for OpenID blacklist
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

SET search_path = esgf_dashboard, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

/* DIMENSION TABLES */
DROP TABLE IF EXISTS esgf_dashboard.cmip6_dim_geolocation CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_dim_date CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_dim_dataset CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_dim_frequency CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_bridge_frequency CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_dim_variable CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_bridge_variable CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_dim_experiment_id CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_bridge_experiment_id CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_dim_realm CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_bridge_realm CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_dim_institution_id CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_bridge_institution_id CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_dim_source_id CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_bridge_source_id CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_fact_download CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_dmart_clients_host_geolocation CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_dmart_experiment_host_time CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_dmart_variable_host_time CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_dmart_dataset_host_time CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_dmart_source_host_time CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_dmart_realm_host_time CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cross_dmart_project_host_geolocation CASCADE;

SET search_path = public, pg_catalog;
