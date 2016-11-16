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

create table esgf_dashboard.aggregation_process (
	hostname character varying(255) PRIMARY KEY,
	lastprocessed_id bigint DEFAULT -1,
	time_stamp timestamp without time zone DEFAULT now() NOT NULL
);

create table esgf_dashboard.federationdw  (al_id bigint, datasetid integer, file_id integer ,project character varying(1024),model character varying(1024),experiment character varying(512), url character varying(1024),mv integer, var character varying(512), realm character varying(512), user_id_hash character varying(512), user_idp character varying(256), year integer, month integer, day  integer, hour integer, service_type character varying(512), remote_addr character varying(128), datasetname character varying(255),time_frequency character varying(512), institute character varying(512),product character varying(512),ensemble character varying(512),cmor_table character varying(512), size bigint,success integer, duration bigint, peername character varying(1024),UNIQUE(al_id,peername));

ALTER TABLE esgf_dashboard.aggregation_process OWNER TO dbsuper;
ALTER TABLE esgf_dashboard.federationdw OWNER TO dbsuper;

SET search_path = public, pg_catalog;
