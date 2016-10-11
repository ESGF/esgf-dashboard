--
-- Update the urls in the dashboard_queue_table
--
set search_path=esgf_dashboard;
select update_url();
