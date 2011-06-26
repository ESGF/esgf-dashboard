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
-- Name: news; Type: TABLE; Schema: public; Owner: esgcet; Tablespace: 
--

CREATE TABLE news (
    idnews integer NOT NULL,
    news character varying(255) NOT NULL,
    datenews timestamp without time zone DEFAULT now() NOT NULL
);


ALTER TABLE public.news OWNER TO esgcet;

--
-- Name: news_idnews_seq; Type: SEQUENCE; Schema: public; Owner: esgcet
--

CREATE SEQUENCE news_idnews_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.news_idnews_seq OWNER TO esgcet;

--
-- Name: news_idnews_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: esgcet
--

ALTER SEQUENCE news_idnews_seq OWNED BY news.idnews;


--
-- Name: news_idnews_seq; Type: SEQUENCE SET; Schema: public; Owner: esgcet
--

SELECT pg_catalog.setval('news_idnews_seq', 3, true);


--
-- Name: idnews; Type: DEFAULT; Schema: public; Owner: esgcet
--

ALTER TABLE news ALTER COLUMN idnews SET DEFAULT nextval('news_idnews_seq'::regclass);


--
-- Data for Name: news; Type: TABLE DATA; Schema: public; Owner: esgcet
--

COPY news (idnews, news, datenews) FROM stdin;
1	first news	2011-03-31 18:14:42.161081
2	second news	2011-04-20 17:33:44.595214
3	third news	2011-04-20 17:33:56.138965
\.


--
-- Name: news_pkey; Type: CONSTRAINT; Schema: public; Owner: esgcet; Tablespace: 
--

ALTER TABLE ONLY news
    ADD CONSTRAINT news_pkey PRIMARY KEY (idnews);


--
-- PostgreSQL database dump complete
--

