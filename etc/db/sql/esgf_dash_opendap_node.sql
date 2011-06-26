--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

SET search_path = public, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: opendap_node; Type: TABLE; Schema: public; Owner: esgcet; Tablespace: 
--

CREATE TABLE opendap_node (
    id bigint NOT NULL,
    url character varying(255) NOT NULL
);


ALTER TABLE public.opendap_node OWNER TO esgcet;

--
-- Data for Name: opendap_node; Type: TABLE DATA; Schema: public; Owner: esgcet
--

COPY opendap_node (id, url) FROM stdin;
29	http://esg-dev1.badc.rl.ac.uk/thredds
31	http://adm02.cmcc.it/thredds
30	http://cmip1.dkrz.de/thredds
9	http://esg-vm-demo03.ccs.ornl.gov/thredds
12	http://esgf-node1.llnl.gov/thredds
13	http://esgf-node3.llnl.gov/thredds
\.


--
-- Name: opendap_node_pkey; Type: CONSTRAINT; Schema: public; Owner: esgcet; Tablespace: 
--

ALTER TABLE ONLY opendap_node
    ADD CONSTRAINT opendap_node_pkey PRIMARY KEY (id);


--
-- Name: opendap_node_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: esgcet
--

ALTER TABLE ONLY opendap_node
    ADD CONSTRAINT opendap_node_id_fkey FOREIGN KEY (id) REFERENCES service_instance(id) ON DELETE CASCADE;


--
-- PostgreSQL database dump complete
--

