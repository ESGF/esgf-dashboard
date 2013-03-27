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

delete from esgf_dashboard.service_instance where port<>80;

create table esgf_dashboard.aggregation_process_planb (
        host character varying(1024) PRIMARY KEY,
        time_stamp timestamp without time zone DEFAULT now() NOT NULL,
	action bigint DEFAULT 0 NOT NULL,
	counter_aggr bigint DEFAULT 0 NOT NULL
);

create table esgf_dashboard.federationdw_planb  (id serial primary key, year integer, month integer, downloads bigint, files bigint, users bigint, gb numeric, host character varying(1024) NOT NULL, time_stamp timestamp without time zone DEFAULT now() NOT NULL);

ALTER TABLE esgf_dashboard.aggregation_process_planb OWNER TO dbsuper;
ALTER TABLE esgf_dashboard.federationdw_planb OWNER TO dbsuper;

SET search_path = public, pg_catalog;
