import sys,glob,os,datetime,json
import psycopg2
import urllib.request
import urllib.error
from datetime import timedelta, date
import subprocess
from multiprocessing import Pool

eu_datanodes = ['esgf-node.cmcc.it','esgf-node2.cmcc.it','esgf1.dkrz.de','esgf2.dkrz.de','esgf3.dkrz.de','data.meteo.unican.es','esgf-data1.ceda.ac.uk','esgf-data2.ceda.ac.uk','esgf-data3.ceda.ac.uk','esgf-data5.ceda.ac.uk','esgf-data6.ceda.ac.uk','esgf-data7.ceda.ac.uk','vesg.ipsl.polytechnique.fr','vesg.ipsl.upmc.fr','esg1.umr-cnrm.fr','esgf.bsc.es','esg-dn1.nsc.liu.se','esg-dn2.nsc.liu.se','noresg.norstore.no','noresg.nird.sigma2.no','eridanus.eoc.dlr.de']

ESGF_projects=['CORDEX', 'NARR_Hydrology', 'ISIMIP3b', 'PMIP3', 'LUCID', 'tracmip', 'input4mips', 'e3sm-supplement', 'GeoMIP', 'psipps', 'MiKlip', 'eucleia', 'CREATE-IP', 'CORDEX-Adjust', 'PMIP3','EUCLIPSE', 'MPI-GE', 'ISIMIP2a', 'ana4MIPs', 'TEST', 'reklies-index', 'uerra', 'CMIP6', 'NEX', 'ACME', 'NEXGDDP', 'hiresireland', 'ISIMIP2b', 'CDAT-sample', 'obs4MIPs', 'clipc', 'BioClim', 'CORDEX-Reklies', 'primavera', 'CMIP3', 'TAMIP', 'cmip3', 'e3sm', 'CMIP5', 'cc4e', 'NARRHydrology', 'monthlyfc', 'specs', 'wind']

def null_error(Url):

	error=None

	Url = Url.replace("//","/")
	Url = Url.replace("%20","")
	n=len(Url.split("/"))

	row_proj=[]
	for p in ESGF_projects:
		if str(p.lower()) in Url.lower():
			row_proj.append(p)

	if not Url.lower().endswith('.nc') and not Url.lower().endswith('.nc4'):
		error="no_nc"
	elif not Url.lower().startswith('/thredds/fileserver/'):
		error="no_thredds"
	elif n>=20:
		error="too_long"
	elif len(row_proj)>1:
		error="more_projects"

	return error


def db_conn():

	conn=None
	try:
		conn = psycopg2.connect( connect_timeout=5, host="", user="", password="", dbname="", port="" )
	except (Exception, psycopg2.Error) as error :
 		print ("Error while connecting to PostgreSQL", error)

	return conn

def insert_KPI_dw(f,conn):

	if conn is not None and conn.closed==0:

		tup=[]
		tot_entries=0
		tot_discarded=0
		with open(f) as fp:
			for line in fp:
				if line.strip():
					tot_entries+=1
					fields = line.strip().split(' ')

					Node = fields[0]
					Country = fields[1]
					Date = int(fields[2])
					ts = datetime.datetime.utcfromtimestamp(Date)
					month=ts.month
					year=ts.year
					day=ts.day
					hour=ts.hour
					Status=fields[6]
					Size=fields[7]
					Duration=fields[8]
					if Duration=="-":
						Duration=None
					if Size=="-":
						Size=None

					if Node in eu_datanodes:
						record_to_insert = [Node,Country,year,month,day,hour,Size,Status,Duration,Date,line.strip(),0]
						tup.append(tuple(record_to_insert))
					else:
						tot_discarded+=1

		QUERY="""insert into isenes3_kpis_dw (hostname, country_code, year, month, day, hour,size_log, status,duration,log_timestamp,entry_log,entry_type) values """

		if tot_discarded<tot_entries:
			try:
				cur = conn.cursor()
				args_str = ','.join(cur.mogrify("(%s, %s, %s, %s, %s, %s, %s, %s,%s, %s, %s, %s)", x).decode('utf-8') for x in tup)
				cur.execute(QUERY+args_str)
				conn.commit()
				insert_count=cur.rowcount
				with open(KPI_log_file, "a") as KPI_file:
					KPI_file.write(str(f)+" --- Total entries: "+str(tot_entries)+" --- NOT EU Discarded: "+str(tot_discarded)+" Inserted: "+str(insert_count)+" --- Status: OK\n")
				cur.close()

			except (Exception, psycopg2.Error) as error :
				conn.rollback()
				with open(KPI_log_file, "a") as KPI_file:
					KPI_file.write(str(f)+" --- Total entries: "+str(tot_entries)+" --- NOT EU Discarded: "+str(tot_discarded)+" Inserted: 0 --- Status: ERROR: "+str(error)+"\n")
				cur.close()
		else:
			with open(KPI_log_file, "a") as KPI_file:
				KPI_file.write(str(f)+ " --- All "+str(tot_entries)+" entires discarded\n")
	else:
		KPI_file.write(str(f)+" --- Status: DB CONNECTION ERROR\n")


def insert_crossproject(line,log_fields,body,conn):

	if conn is not None and conn.closed==0:

		Host = log_fields[0]
		Country = log_fields[1]
		Date=int(log_fields[2])
		ts = datetime.datetime.utcfromtimestamp(Date)
		Day=ts.day
		Month=ts.month
		Year=ts.year
		Hour=ts.hour
		Url = log_fields[4]
		Status=log_fields[6]
		Size_Log=log_fields[7]
		Duration=log_fields[8]

		if Duration=="-":
			Duration=None
		if Size_Log=="-":
			Size_Log=None

		Replica=body["docs"][0]["replica"]
		Size_Solr=body["docs"][0]["size"]
		Project=body["docs"][0]["project"][0]

		QUERY="insert into cross_project_dw (size_solr, size_log, status, duration, replica, log_timestamp, hour, day, month, year, hostname, country_code, entry_log, entry_type, project,entry_timestamp) values (%s,%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s,%s,%s,%s, %s) RETURNING id;"
		record = (Size_Solr,Size_Log,Status,Duration,Replica,Date,Hour,Day,Month,Year,Host,Country,line.strip(),0,Project,datetime.datetime.now())

		try:
			cur = conn.cursor()
			cur.execute(QUERY,record)
			fk = cur.fetchone()[0]
			conn.commit()
			cur.close()

			return str(0)+";ok;"+str(fk)

		except (Exception, psycopg2.Error) as error :
			conn.rollback()
			cur.close()

			return str(-4)+";"+str(error)
	else:
		return str(-3)+";db_conn_not_set"



def insert_crossproject_null(line,log_fields,conn):

        if conn is not None and conn.closed==0:

                Host = log_fields[0]
                Url = log_fields[4]

                e=null_error(Url)
                QUERY = "insert into discarded_ids (hostname,entry_log,error,entry_timestamp,entry_type) values (%s,%s, %s, %s, %s);"
                record = (Host,line.strip(),e,datetime.datetime.now(),0)
                try:
                        cur = conn.cursor()
                        cur.execute(QUERY,record)
                        conn.commit()
                        cur.close()
                        return str(0)+";ok"
                except (Exception, psycopg2.Error) as error :
                        conn.rollback()
                        cur.close()
                        return str(-4)+";"+str(error)
        else:
                return str(-3)+";db_conn_not_set"



def insert_cmip5(line,log_fields,body,conn,fk):

	if conn is not None and conn.closed==0:

		Host = log_fields[0]
		Country = log_fields[1]
		Date=int(log_fields[2])
		ts = datetime.datetime.utcfromtimestamp(Date)
		Day=ts.day
		Month=ts.month
		Year=ts.year
		Hour=ts.hour
		Status=log_fields[6]
		Size_Log=log_fields[7]
		Duration=log_fields[8]
		Service_type=log_fields[9]

		if Duration=="-":
			Duration=None
		if Size_Log=="-":
			Size_Log=None
		
		Replica=body["docs"][0]["replica"]
		Size_Solr=body["docs"][0]["size"]
		Institute=body["docs"][0]["institute"][0]

		Dataset_id=body["docs"][0]["dataset_id"]
		Variable_code=body["docs"][0]["variable"][0]
		Cf_standard_name=body["docs"][0]["cf_standard_name"][0]
		Variable_long_name=body["docs"][0]["variable_long_name"][0]
		Realm=body["docs"][0]["realm"][0]
		Model=body["docs"][0]["model"][0]
		Experiment=body["docs"][0]["experiment"][0]
		Time_frequency=body["docs"][0]["time_frequency"][0]
		File_version=body["docs"][0]["version"]
		
		QUERY="insert into cmip5_project_specific_dw (cross_project_id,size_solr, size_log,status, duration, replica, log_timestamp,hostname, country_code, hour,day, month, year, institute, realm,file_version, dataset_id, model, experiment, variable_code, variable_long_name, cf_standard_name, time_frequency, service_type,entry_log,entry_type,entry_timestamp) values (%s,%s,%s, %s, %s,%s,%s,%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s,%s,%s);"
		record = (fk,Size_Solr,Size_Log,Status,Duration,Replica,Date,Host,Country,Hour,Day,Month,Year,Institute,Realm,File_version,Dataset_id,Model,Experiment,Variable_code,Variable_long_name,Cf_standard_name,Time_frequency,Service_type,line.strip(),0,datetime.datetime.now())

		try:
			cur = conn.cursor()
			cur.execute(QUERY,record)
			conn.commit()
			cur.close()
			return str(0)+";ok"

		except (Exception, psycopg2.Error) as error :
			conn.rollback()
			cur.close()
			return str(-5)+";"+str(error)
	else:
		return str(-3)+";db_conn_not_set"

def insert_cmip6(line,log_fields,body,conn,fk):

        if conn is not None and conn.closed==0:

                Host = log_fields[0]
                Country = log_fields[1]
                Date=int(log_fields[2])
                ts = datetime.datetime.utcfromtimestamp(Date)
                Day=ts.day
                Month=ts.month
                Year=ts.year
                Hour=ts.hour
                Status=log_fields[6]
                Size_Log=log_fields[7]
                Duration=log_fields[8]
                Service_type=log_fields[9]
                
                if Duration=="-":
                        Duration=None
                if Size_Log=="-":
                        Size_Log=None
                
                Replica=body["docs"][0]["replica"]
                Size_Solr=body["docs"][0]["size"]
                Institute=body["docs"][0]["institution_id"][0]

                Dataset_id=body["docs"][0]["dataset_id"]
                Variable_code=body["docs"][0]["variable"][0]
                Cf_standard_name=body["docs"][0]["cf_standard_name"][0]
                Variable_long_name=body["docs"][0]["variable_long_name"][0]
                Realm=body["docs"][0]["realm"][0]
                Model=body["docs"][0]["source_id"][0]
                Experiment=body["docs"][0]["experiment_id"][0]
                Time_frequency=body["docs"][0]["frequency"][0]
                File_version=body["docs"][0]["version"]

                QUERY="insert into cmip6_project_specific_dw (cross_project_id,size_solr,size_log,status, duration, replica, hostname, country_code, log_timestamp,hour,day, month, year, institution_id, realm,file_version, dataset_id, source_id, experiment_id, variable_code, variable_long_name, cf_standard_name, frequency, service_type,entry_log,entry_type,entry_timestamp) values (%s,%s,%s, %s, %s,%s,%s,%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s,%s,%s);"
                record = (fk,Size_Solr,Size_Log,Status,Duration,Replica,Host,Country,Date,Hour,Day,Month,Year,Institute,Realm,File_version,Dataset_id,Model,Experiment,Variable_code,Variable_long_name,Cf_standard_name,Time_frequency,Service_type,line.strip(),0,datetime.datetime.now())

                try:
                        cur = conn.cursor()
                        cur.execute(QUERY,record)
                        conn.commit()
                        cur.close()
                        return str(0)+";ok"

                except (Exception, psycopg2.Error) as error :
                        conn.rollback()
                        cur.close()
                        return str(-5)+";"+str(error)
        else:
                return str(-3)+";db_conn_not_set"



def insert_obs4mips(line,log_fields,body,conn,fk):

        if conn is not None and conn.closed==0:

                Host = log_fields[0]
                Country = log_fields[1]
                Date=int(log_fields[2])
                ts = datetime.datetime.utcfromtimestamp(Date)
                Day=ts.day
                Month=ts.month
                Year=ts.year
                Hour=ts.hour
                Status=log_fields[6]
                Size_Log=log_fields[7]
                Duration=log_fields[8]
                Service_type=log_fields[9]
                if Duration=="-":
                        Duration=None
                if Size_Log=="-":
                        Size_Log=None
                
                Replica=body["docs"][0]["replica"]
                Size_Solr=body["docs"][0]["size"]
                
                try:
                        Institute=body["docs"][0]["institution_id"][0]
                except:
                        Institute=body["docs"][0]["institute"][0]

                Dataset_id=body["docs"][0]["dataset_id"]
                Variable_code=body["docs"][0]["variable"][0]
                Cf_standard_name=body["docs"][0]["cf_standard_name"][0]
                Variable_long_name=body["docs"][0]["variable_long_name"][0]
                
                try:
                        Time_frequency=body["docs"][0]["frequency"][0]
                except:
                        Time_frequency=body["docs"][0]["time_frequency"][0]

                File_version=body["docs"][0]["version"]
                Realm=body["docs"][0]["realm"][0]
                Source_id=body["docs"][0]["source_id"][0]
                
                QUERY="insert into obs4mips_project_specific_dw (cross_project_id,size_solr,size_log, status, duration, replica, hostname, country_code, log_timestamp,hour,day, month, year, institute, file_version, dataset_id, source_id, realm, variable_code, variable_long_name, cf_standard_name, time_frequency, service_type,entry_log,entry_type,entry_timestamp) values (%s,%s,%s, %s, %s,%s,%s,%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s,%s);"

                record = (fk,Size_Solr,Size_Log,Status,Duration,Replica,Host,Country,Day,Hour,Day,Month,Year,Institute,File_version,Dataset_id,Source_id,Realm,Variable_code,Variable_long_name,Cf_standard_name,Time_frequency,Service_type,line.strip(),0,datetime.datetime.now())

                try:
                        cur = conn.cursor()
                        cur.execute(QUERY,record)
                        conn.commit()
                        cur.close()
                        return str(0)+";ok"

                except (Exception, psycopg2.Error) as error :
                        conn.rollback()
                        cur.close()
                        return str(-5)+";"+str(error)
        else:
                return str(-3)+";db_conn_not_set"




def insert_cordex(line,log_fields,body,conn,fk):

        if conn is not None and conn.closed==0:

                Host = log_fields[0]
                Country = log_fields[1]
                Date=int(log_fields[2])
                ts = datetime.datetime.utcfromtimestamp(Date)
                Day=ts.day
                Month=ts.month
                Year=ts.year
                Hour=ts.hour
                Status=log_fields[6]
                Size_Log=log_fields[7]
                Duration=log_fields[8]
                Service_type=log_fields[9]
                if Duration=="-":
                        Duration=None
                if Size_Log=="-":
                        Size_Log=None

                Replica=body["docs"][0]["replica"]
                Size_Solr=body["docs"][0]["size"]
                Institute=body["docs"][0]["institute"][0]

                Dataset_id=body["docs"][0]["dataset_id"]
                Variable_code=body["docs"][0]["variable"][0]
                Cf_standard_name=body["docs"][0]["cf_standard_name"][0]
                Variable_long_name=body["docs"][0]["variable_long_name"][0]
                Driving_model=body["docs"][0]["driving_model"][0]
                Experiment=body["docs"][0]["experiment"][0]
                Time_frequency=body["docs"][0]["time_frequency"][0]
                File_version=body["docs"][0]["version"]
                Domain=body["docs"][0]["domain"][0]
                Rcm_name=body["docs"][0]["rcm_name"][0]
                Rcm_version=body["docs"][0]["rcm_version"][0]

                QUERY="insert into cordex_project_specific_dw (cross_project_id,size_solr,size_log, status, duration, replica, hostname, country_code, log_timestamp,hour,day, month, year, institute, file_version, domain, rcm_name, rcm_version, dataset_id, driving_model, experiment, variable_code, variable_long_name, cf_standard_name, time_frequency, service_type,entry_log,entry_type,entry_timestamp) values (%s,%s,%s, %s, %s,%s,%s,%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s,%s,%s,%s,%s);"
                record = (fk,Size_Solr,Size_Log,Status,Duration,Replica,Host,Country,Date,Hour,Day,Month,Year,Institute,File_version,Domain,Rcm_name,Rcm_version,Dataset_id,Driving_model,Experiment,Variable_code,Variable_long_name,Cf_standard_name,Time_frequency,Service_type,line.strip(),0,datetime.datetime.now())

                try:
                        cur = conn.cursor()
                        cur.execute(QUERY,record)
                        conn.commit()
                        cur.close()
                        return str(0)+";ok"

                except (Exception, psycopg2.Error) as error :
                        conn.rollback()
                        cur.close()
                        return str(-5)+";"+str(error)
        else:
               return str(-3)+";db_conn_not_set"     



def process(args):

	files=args[0]
	rank=args[1]

	total_size=0
	for f in files:
		total_size+=os.path.getsize(f)

	conn = db_conn()
	
	total_size = total_size / 1024 / 1024

	for f in files:
		filename=os.path.basename(f)

		insert_KPI_dw(f,conn)

		with open(f) as fp:
			for line in fp:
				if line.strip():

					log_fields = line.strip().split(' ')
					
					Host = log_fields[0]
					Url = log_fields[4]

					esg_search_url=url_pre+"http://"+str(Host)+str(Url)+url_post
					
					try:
						response = urllib.request.urlopen(esg_search_url)
						charset=response.info().get_content_charset()
						content = response.read().decode(charset)
						data = json.loads(content)
						body=data["response"]
						found=body["numFound"]

						if found == 1:
							project=body["docs"][0]["project"][0]
							
							s = insert_crossproject(line,log_fields,body,conn)
						
							e_code=s.split(";")[0]
							e_desc=s.split(";")[1]
					
							if e_code=="0":
								fk=s.split(";")[2]
								if project.upper()=="CMIP5":
									s=insert_cmip5(line,log_fields,body,conn,fk)
								elif project.upper()=="CMIP6":
									s=insert_cmip6(line,log_fields,body,conn,fk)
								elif project.upper()=="CORDEX":
									s=insert_cordex(line,log_fields,body,conn,fk)
								elif project.upper()=="OBS4MIPS":
									s=insert_obs4mips(line,log_fields,body,conn,fk)
								else:
									with open(other_project_path+"/"+filename, "a") as other_project_file:
										other_project_file.write(line.strip()+"\n")
									continue
							
								e_code=s.split(";",1)[0]
								e_desc=s.split(";",1)[1]						
			
								if e_code=="0":
									with open(archive_path+"/"+filename, "a") as archive_file:
										archive_file.write(line.strip()+"\n")
								else:
									with open(err_path+"/"+filename, "a") as err_file:
										err_file.write(line.strip()+" --- "+str(e_desc)+" --- "+str(e_code)+"\n")
									with open(output_path+"/"+filename, "a") as output_file:
										output_file.write(line.strip()+"\n")
							else:
								with open(err_path+"/"+filename, "a") as err_file:
									err_file.write(line.strip()+" --- "+str(e_desc)+" --- "+str(e_code)+"\n")
								with open(output_path+"/"+filename, "a") as output_file:
									output_file.write(line.strip()+"\n")
						else:
							s = insert_crossproject_null(line,log_fields,conn)
							e_code=s.split(";")[0]
							e_desc=s.split(";")[1]
							if e_code=="0":
								with open(null_path+"/"+filename, "a") as null_file:
									null_file.write(line.strip()+"\n")
							else:
								with open(err_path+"/"+filename, "a") as err_file:
									err_file.write(line.strip()+" --- "+str(e_desc)+" --- "+str(e_code)+"\n")
								with open(output_path+"/"+filename, "a") as output_file:
									output_file.write(line.strip()+"\n")
	
					except urllib.error.URLError as e:
						with open(err_path+"/"+filename, "a") as err_file:
							err_file.write(line.strip()+" --- "+str(e)+" --- -1\n")
						with open(output_path+"/"+filename, "a") as output_file:
							output_file.write(line.strip()+"\n")
		
					except Exception as e2:
						with open(err_path+"/"+filename, "a") as err_file:
							err_file.write(line.strip()+" --- " +str(e2)+"  --- -1\n")
						with open(output_path+"/"+filename, "a") as output_file:
							output_file.write(line.strip()+"\n")
					except:
						with open(err_path+"/"+filename, "a") as err_file:
							err_file.write(line.strip()+" --- Generic error --- -1\n")
						with open(output_path+"/"+filename, "a") as output_file:
							output_file.write(line.strip()+"\n")

		os.system("mv "+f+" "+done_path)

	conn.close()





if len(sys.argv) < 2:
        print("Date (yyyyMMdd format) required!")
        sys.exit(0)

start_date=sys.argv[1]
dt = datetime.datetime.strptime(start_date, '%Y%m%d')

root_path="/path/to/realtime/dir/"

url_pre="http://localhost/esg-search/search?type=File&format=application%2Fsolr%2Bjson&url="
url_post="|application/netcdf|HTTPServer"
file_pattern="l3*.txt"

KPI_log_file="/path/to/KPI/logfile"

y=dt.year
m='{:02d}'.format(dt.month)
d='{:02d}'.format(dt.day)

input_path=root_path+str(y)+"/"+str(m)+"/"+str(d)+"/input"
err_path=root_path+str(y)+"/"+str(m)+"/"+str(d)+"/err"
output_path=root_path+str(y)+"/"+str(m)+"/"+str(d)+"/output"
archive_path=root_path+str(y)+"/"+str(m)+"/"+str(d)+"/archive"
done_path=root_path+str(y)+"/"+str(m)+"/"+str(d)+"/done"
other_project_path=root_path+str(y)+"/"+str(m)+"/"+str(d)+"/oth_proj"
null_path=root_path+str(y)+"/"+str(m)+"/"+str(d)+"/null_proj"

files = glob.glob(os.path.join(input_path,file_pattern))

os.system("mkdir -p "+err_path)
os.system("mkdir -p "+output_path)
os.system("mkdir -p "+archive_path)
os.system("mkdir -p "+done_path)
os.system("mkdir -p "+other_project_path)
os.system("mkdir -p "+null_path)

p = Pool(processes=24)
p.map(process,[([f],i) for i,f in enumerate(files)])
p.close()
p.join()

