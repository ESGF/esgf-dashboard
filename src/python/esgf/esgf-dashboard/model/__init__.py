"ESGF Database class, table, and object/relational mapping definitions."

from sqlalchemy import Column, MetaData, Table, types, ForeignKey, UniqueConstraint, Index, ForeignKeyConstraint
from sqlalchemy.orm import mapper, relation

metadata = MetaData()

# esgf_security tables

userTable = Table('user', metadata,
                  Column('id', types.Integer, primary_key=True, autoincrement=True),
                  Column('firstname', types.String(100), nullable=False),
                  Column('lastname', types.String(100), nullable=False),
                  Column('email', types.String(100), nullable=False),
                  Column('username', types.String(100), unique=True, nullable=False),
                  Column('password', types.String(100)),
                  Column('dn', types.String(300)),
                  Column('openid', types.String(200), index=True, nullable=False),
                  Column('organization', types.String(200)),
                  Column('city', types.String(100)),
                  Column('state', types.String(100)),
                  Column('country', types.String(100)),
                  schema = 'esgf_security',
                  )

groupTable = Table('group', metadata,
                   Column('id', types.Integer, primary_key=True, autoincrement=True),
                   Column('name', types.String(100), unique=True, nullable=False),
                   Column('description', types.Text, nullable=False),
                   Column('visible', types.Boolean, default=True),
                   Column('automatic_approval', types.Boolean, default=False),
                   schema = 'esgf_security',
                   )

roleTable = Table('role', metadata,
                  Column('id', types.Integer, primary_key=True),
                  Column('name', types.String(100), unique=True, nullable=False),
                  Column('description', types.Text, nullable=False),
                  schema = 'esgf_security',
                  )

permissionTable = Table('permission', metadata,
                        Column('user_id', types.Integer, ForeignKey('esgf_security.user.id'), primary_key=True),
                        Column('group_id', types.Integer, ForeignKey('esgf_security.group.id'), primary_key=True),
                        Column('role_id', types.Integer, ForeignKey('esgf_security.role.id'), primary_key=True),
                        schema = 'esgf_security',
                        )

# esgf_node_manager tables

accessLoggingTable = Table('access_logging', metadata,
                           Column('id', types.Integer, primary_key=True, autoincrement=True),
                           Column('user_id', types.String(), nullable=False),
                           Column('email', types.String()), 
                           Column('url', types.String(), index=True, nullable=False),
                           Column('file_id', types.String()), 
                           Column('remote_addr', types.String(), nullable=False),
                           Column('user_agent', types.String()), 
                           Column('service_type', types.String()), 
                           Column('batch_update_time', types.Float(precision=53)),
                           Column('date_fetched', types.Float(precision=53), nullable=False),
                           Column('success', types.Boolean, default=False), 
                           Column('duration', types.Float(precision=53), default=0.0),
                           schema = 'esgf_node_manager',
                           )

downloadTable = Table('download', metadata,
                      Column('userid', types.String(64)),
                      Column('url', types.String(255)),
                      schema = 'esgf_node_manager',
                      )

metricsRunLogTable = Table('metrics_run_log', metadata,
                           Column('id', types.Integer, primary_key=True, autoincrement=True),
                           Column('last_run_time', types.Float(precision=53)),
                           schema = 'esgf_node_manager',
                           )

monitorRunLogTable = Table('monitor_run_log', metadata,
                           Column('id', types.Integer, primary_key=True, autoincrement=True),
                           Column('last_run_time', types.Float(precision=53)),
                           schema = 'esgf_node_manager',
                           )

notificationRunLogTable = Table('notification_run_log', metadata, 
                                Column('id', types.Integer, primary_key=True, autoincrement=True),
                                Column('notify_time', types.Float(precision=53)),
                                schema = 'esgf_node_manager',
                                )

# Mapped classes

class User(object):

    def __init__(self, firstname, lastname, email, username, openid, password=None, dn=None, organization=None, city=None, state=None, country=None):
        self.firstname = firstname
        self.lastname = lastname
        self.email = email
        self.username = username
        self.password = password
        self.dn = dn
        self.openid = openid
        self.organization = organization
        self.city = city
        self.state = state
        self.country = country        

    def __repr__(self):
        return "<User %s, %s %s, %s>"%(self.username, self.firstname, self.lastname, self.email)

class Group(object):

    def __init__(self, name, description, visible=True, automatic_approval=False):
        self.name = name
        self.description = description
        self.visible = visible
        self.automatic_approval = automatic_approval

    def __repr__(self):
        return "<Group %s: %s>"%(self.name, self.description)

class Role(object):

    def __init__(self, id, name, description):
        self.id = id
        self.name = name
        self.description = description

    def __repr__(self):
        return "<Role %s: %s>"%(self.name, self.description)

class Permission(object):

    def __init__(self, user_id, group_id, role_id):
        self.user_id = user_id
        self.group_id = group_id
        self.role_id = role_id

    def __repr__(self):
        return "<Permission: user %d has role %d in group %d>"%(self.user_id, self.role_id, self.group_id)

mapper(User, userTable, properties={'permissions':relation(Permission, backref='user', cascade="all, delete, delete-orphan")})
mapper(Group, groupTable, properties={'permissions':relation(Permission, backref='group', cascade="all, delete, delete-orphan")})
mapper(Role, roleTable)
mapper(Permission, permissionTable)
