--
-- PostgreSQL database dump
--

SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

--
-- Clean up existing tables from public schema if present
--
DROP TABLE IF EXISTS access_logging;
DROP TABLE IF EXISTS download;
DROP TABLE IF EXISTS metrics_run_log;
DROP TABLE IF EXISTS monitor_run_log;
DROP TABLE IF EXISTS notification_run_log;

--
-- Name: esgf_node_manager; Type: SCHEMA; Schema: -; Owner: -
--

CREATE SCHEMA esgf_node_manager;


SET search_path = esgf_node_manager, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: access_logging; Type: TABLE; Schema: esgf_node_manager; Owner: -; Tablespace: 
--

CREATE TABLE access_logging (
    id integer NOT NULL,
    user_id character varying NOT NULL,
    email character varying,
    url character varying NOT NULL,
    file_id character varying,
    remote_addr character varying NOT NULL,
    user_agent character varying,
    service_type character varying,
    batch_update_time double precision,
    date_fetched double precision NOT NULL,
    success boolean,
    duration double precision,
    user_id_hash character varying,
    user_idp character varying
);


--
-- Name: download; Type: TABLE; Schema: esgf_node_manager; Owner: -; Tablespace: 
--

CREATE TABLE download (
    userid character varying(64),
    url character varying(255)
);


--
-- Name: metrics_run_log; Type: TABLE; Schema: esgf_node_manager; Owner: -; Tablespace: 
--

CREATE TABLE metrics_run_log (
    id character varying NOT NULL,
    last_run_time double precision
);


--
-- Name: monitor_run_log; Type: TABLE; Schema: esgf_node_manager; Owner: -; Tablespace: 
--

CREATE TABLE monitor_run_log (
    id character varying NOT NULL,
    last_run_time double precision
);


--
-- Name: notification_run_log; Type: TABLE; Schema: esgf_node_manager; Owner: -; Tablespace: 
--

CREATE TABLE notification_run_log (
    id character varying NOT NULL,
    notify_time double precision
);


--
-- Name: access_logging_id_seq; Type: SEQUENCE; Schema: esgf_node_manager; Owner: -
--

CREATE SEQUENCE access_logging_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;

--
-- Name: access_logging_id_seq; Type: SEQUENCE OWNED BY; Schema: esgf_node_manager; Owner: -
--

ALTER SEQUENCE access_logging_id_seq OWNED BY access_logging.id;

--
-- Name: id; Type: DEFAULT; Schema: esgf_node_manager; Owner: -
--

ALTER TABLE access_logging ALTER COLUMN id SET DEFAULT nextval('access_logging_id_seq'::regclass);


--
-- Name: access_logging_pkey; Type: CONSTRAINT; Schema: esgf_node_manager; Owner: -; Tablespace: 
--

ALTER TABLE ONLY access_logging
    ADD CONSTRAINT access_logging_pkey PRIMARY KEY (id);

--
-- Name: ix_esgf_node_manager_access_logging_url; Type: INDEX; Schema: esgf_node_manager; Owner: -; Tablespace: 
--

CREATE INDEX ix_esgf_node_manager_access_logging_url ON access_logging USING btree (url);

--
-- PostgreSQL database dump complete
-- Reset search path to public, so that esgf_migrate_version can be updated.
--

SET search_path = public, pg_catalog;

