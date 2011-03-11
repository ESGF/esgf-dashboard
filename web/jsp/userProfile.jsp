<%-- AUTHOR: University of Salento and CMCC --%>
<%@ page language="java" contentType="text/html; charset=ISO-8859-1"
    pageEncoding="ISO-8859-1"%>
<%@ taglib prefix="s" uri="/struts-tags" %>

<html>
	<head>
		<s:include value="commons/homeLayout.jsp" />
		<script type="text/javascript">
		//<![CDATA[
			Ext.onReady(function() {
				buildLayout(
					new Ext.Panel({
		        		region: 'center',
		        		autoHeight: true,
		    			bodyStyle: 'paddingBottom: 5px; marginTop: 50px;',
		    			layout: "hbox",
    					layoutConfig: {
    					 	 pack: 'center',
    					     align: 'top'		                    	                        
                        },
		    			defaults: {
		    				msgTarget : 'side'				
		    	        },
						items: [
							new Ext.form.FormPanel({								
								id: "profileForm",
								title: '<s:text name="userProfile.form.title" />',								
							    labelWidth: 120, 
							    url: '<s:url action="EditUserProfile" namespace="/secure" encode="false" />',
							    border: false,
							    frame: false,
							    waitMsgTarget: true,									        
							    bodyStyle: 'padding:5px 5px 0',
							    width: 410,
							    defaults: {width: 250},
							    defaultType: 'textfield',
							    items: [{
							            fieldLabel: '<s:text name="userProfile.form.firstName" />*',
							            name: 'user.name',
							            allowBlank: false,
							            maxLength: 45, 
										selectOnFocus: true,
										value: '<s:property value="user.name" />'
							        }, {
							            fieldLabel: '<s:text name="userProfile.form.lastName" />*',
							            name: 'user.surname',
							            allowBlank: false,
							            maxLength: 45, 
										selectOnFocus: true,
										value: '<s:property value="user.surname" />'
							        }, new Ext.form.ComboBox({
							        	listWidth: 250,
							            mode: 'local',			                        
							            triggerAction: 'all',
							            forceSelection: true,
							            selectOnFocus: true,
							            typeAhead: true,                    
							            editable: true,
							            displayField: 'name',
							            valueField: 'value',
							            store: new Ext.data.JsonStore({
							            	fields: [ 'name', 'value' ],
							                data: [
							                	// <s:iterator value="countries">
							                	{ name: '<s:property value="name" />', value: <s:property value="id" /> },
												// </s:iterator>
							                ]
							            }),
							            value: <s:property value="user.country" default="null" />,
							            fieldLabel: '<s:text name="userProfile.form.country" />',
							            name: 'user.country',
							            hiddenName: 'user.country',
							            allowBlank: true						               
							        }), {
								        id: 'userDnField',
							        	fieldLabel: '<s:text name="userProfile.form.userdn" />',
							        	value: '<s:property value="user.dn" />',
							        	name: "user.dn",
							        	allowBlank: true,
							        	selectOnFocus: true,
							        	validationEvent: 'change',
										validateOnBlur: false,
										validator: function(value) {
											if(value.trim().toLowerCase() == '<s:property value="user.dn" />'.toLowerCase())
												return true;										
											Ext.Ajax.request({
												url: '<s:url action="UserDnValidation" namespace="/secure" encode="false" />',
												success: function(response, opts) {
													if(!eval(response.responseText))
														Ext.getCmp("userDnField").markInvalid("User DN just in use");															
												},
												failure: function() {},
												scope: this,
												params: { dn: value }
											});
											return true;
						            	}
							        },
							        {
										fieldLabel: '<s:text name="userProfile.form.username" />',
										submitValue: false,
										value: '<s:property value="user.username" />',
										disabled: true							
								    }, {
										fieldLabel: '<s:text name="userProfile.form.mail" />',
										submitValue: false,
										value: '<s:property value="user.mail" />',
										disabled: true							
								    },
							        {
							            id: 'passwordField',
							            fieldLabel: '<s:text name="userProfile.form.password" />',
							            name: 'user.password',
							            allowBlank: true,
							            maxLength: 15, 
							            minLength: 8,
							            inputType: 'password',
										selectOnFocus: true										
							        }, {
							            fieldLabel: '<s:text name="userProfile.form.passwordConfirm" />',
							            allowBlank: true,
							            submitValue: false,
							            maxLength: 15,
							            minLength: 8, 
							            inputType: 'password',
										selectOnFocus: true,
										validator: function(value) {
											var passwordField = Ext.getCmp("passwordField");
											if(passwordField.isValid() && passwordField.getValue()==value)
												return true;
											return "Password not valid";
							        	}
							        }
							    ],
							    buttonAlign: "center",
							    buttons: [{
							        id: "buttonEditProfile",
							        text: '<s:text name="userProfile.form.change" />',
							        handler: function() {
							        	Ext.getCmp("buttonEditProfile").setDisabled(true);
							        	Ext.getCmp("buttonClear").setDisabled(true);
							    		Ext.getCmp("profileForm").getForm().submit({
							    			clientValidation: true,
							        		waitMsg: '<s:text name="userProfile.form.submitMsg" />',
							        		success: function(form, action) {
								    			Ext.getCmp("buttonEditProfile").setDisabled(false);
							        			Ext.getCmp("buttonClear").setDisabled(false);							        			
							        			if(eval(action.response.responseText))
							        				Ext.MessageBox.alert('Status', 'Success');
							        			else
							        				Ext.MessageBox.alert('Status', 'Error');
							        		},
							        		failure: function(form, action) {							        			
							        			Ext.getCmp("buttonEditProfile").setDisabled(false);
							        			Ext.getCmp("buttonClear").setDisabled(false);
							        			Ext.MessageBox.alert('Status', 'Error');
							        		},							        		
							        		timeout: 60
							        	});
							    	}			            
							    }, {
							        id: "buttonClear",
							        text: '<s:text name="userProfile.form.clear" />',
							        handler: function() {
							            Ext.getCmp("profileForm").getForm().reset();
							            Ext.getCmp("profileForm").getForm().clearInvalid();
							        }
							    }]
							})						 
						]
					})
				);
			});
		//]]>
		</script>		
	</head>
	<body>
		<s:include value="commons/header.jsp" />
	</body>
</html>
