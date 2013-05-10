--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

SET search_path = esgf_dashboard, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

alter table esgf_dashboard.host add status integer;
alter table esgf_dashboard.host add elapsedtime bigint;

SET search_path = public, pg_catalog;
