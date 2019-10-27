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
DROP TABLE IF EXISTS esgf_dashboard.downloads_info CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.registry_collector CASCADE;

SET search_path = public, pg_catalog;
