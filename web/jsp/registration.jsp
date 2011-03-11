<%-- AUTHOR: University of Salento and CMCC --%>
<%@ page language="java" contentType="text/html; charset=ISO-8859-1"
    pageEncoding="ISO-8859-1"%>
<%@ taglib prefix="s" uri="/struts-tags" %>

<html>
	<head>
		<s:include value="commons/homeLayout.jsp" />
		<link rel="stylesheet" type="text/css" href="../css/MultiSelect.css"/>		
		<style type="text/css">
		<!--
			.x-list-selected {
				background: white;
			}
		-->
		</style>
		<script type="text/javascript" src="../js/ext/ux/MultiSelect.js"></script>
    	<script type="text/javascript" src="../js/ext/ux/ItemSelector.js"></script>  		
		
		<script type="text/javascript">
		//<![CDATA[
			var idCaptcha = 0;
			function reloadCaptcha() {
				var node = document.getElementById("captcha");
				node.setAttribute("src", '<s:url action="Captcha" namespace="/" encode="false" />?id='+idCaptcha);	
				idCaptcha++;
			}
			
			Ext.onReady(function() {
				buildLayout(
					new Ext.Panel({
		        		region: 'center',		        		
		    			bodyStyle: 'paddingBottom: 5px; marginTop: 20px;',
		    			autoScroll: true,
		    			layout: "hbox",
    					layoutConfig: {
    					 	 pack: 'center',
    					     align: 'top'		                    	                        
                        },		    			
		    			defaults: {
		    				msgTarget : 'side'
		    	        },
						items: [
							/*new Ext.Panel({
								width: 400,
							    html: '<br/><p class="registrationInfo">Join to ESGF Dashboard</p><br><img src="../img/grid-computing.jpg" alt="" />'
							}),
							new Ext.Panel({width: 50}),*/
							new Ext.form.FormPanel({																
								id: "registrationForm",
								title: '<s:text name="registration.form.title" />',								
						        labelWidth: 120, 
						        width: 480, //960,
						        url: '<s:url action="Registration" namespace="/secure" encode="false" />',
						        border: false,
						        frame: false,
						        waitMsgTarget: true,
						        /*layout: "hbox",
		    					layoutConfig: {
		    					 	 pack: 'center',
		    					     align: 'top'		                    	                        
		                        },*/					        								        
						        bodyStyle: 'padding:5px 5px 0',
						        defaultType: 'textfield',
						        defaults: {width: 330},
						        items: [
					              	{
						                fieldLabel: '<s:text name="registration.form.firstName" />*',
						                name: 'user.name',
						                allowBlank: false,
						                maxLength: 45, 
										selectOnFocus: true
						            }, {
						                fieldLabel: '<s:text name="registration.form.lastName" />*',
						                name: 'user.surname',
						                allowBlank: false,
						                maxLength: 45, 
										selectOnFocus: true
						            }, new Ext.form.ComboBox({
						            	listWidth: 330,
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
						                fieldLabel: '<s:text name="registration.form.country" />',
						                name: 'user.country',
						                hiddenName: 'user.country',
						                allowBlank: true						               
						            }), {
							            id: 'userDnField',
						            	fieldLabel: '<s:text name="registration.form.userdn" />',
							        	value: '<s:property value="user.dn" />',
							        	name: "user.dn",
							        	allowBlank: true,
							        	selectOnFocus: true,
							        	validationEvent: 'change',
										validateOnBlur: false,
										validator: function(value) {													
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
							            id: "mailField",
						                fieldLabel: '<s:text name="registration.form.mail" />*',
						                name: 'user.mail',
						                allowBlank: false,
						                vtype:'email',
						                maxLength: 45, 
										selectOnFocus: true
						            }, /*{
						                fieldLabel: 'Email (confirm)',								                
						                allowBlank: false,
						                submitValue: false,
						                vtype:'email',
						                maxLength: 45, 
										selectOnFocus: true, 
										validator: function(value) {
											var mailField = Ext.getCmp("mailField");
											if(mailField.isValid() && mailField.getValue()==value)
												return true;
											return "Mail not valid";
						            	}
						            },*/{
							            id: 'usernameField',
						                fieldLabel: '<s:text name="registration.form.username" />*',
						                name: 'user.username',
						                allowBlank: false,
						                maxLength: 32, 
						                minLength: 6,
										selectOnFocus: true,
										validationEvent: 'change',
										validateOnBlur: false,
										validator: function(value) {																
											Ext.Ajax.request({
												url: '<s:url action="UsernameValidation" namespace="/secure" encode="false" />',
												success: function(response, opts) {
													if(!eval(response.responseText))
														Ext.getCmp("usernameField").markInvalid("Username just in use");															
												},
												failure: function() {},
												scope: this,
												params: { username: value }
											});
											return true;
						            	}
						            }, {
							            id: 'passwordField',
						                fieldLabel: '<s:text name="registration.form.password" />*',
						                name: 'user.password',
						                allowBlank: false,
						                maxLength: 25, 
						                minLength: 6,
						                inputType: 'password',
										selectOnFocus: true
						            }, {
						                fieldLabel: '<s:text name="registration.form.passwordConfirm" />*',
						                allowBlank: false,
						                submitValue: false,
						                maxLength: 25,
						                minLength: 6, 
						                inputType: 'password',
										selectOnFocus: true,
										validator: function(value) {
											var passwordField = Ext.getCmp("passwordField");
											if(passwordField.isValid() && passwordField.getValue()==value)
												return true;
											return "Password not valid";
						            	}
						            }, 
						            new Ext.Panel({
							            layout: 'fit',
							            fieldLabel: '<s:text name="registration.form.joinProject" />',					            							            		   
									    items: [
								            {
									            id: 'itemselector',
											    xtype: 'itemselector',
											    name: 'idProjects',									    
											    imagePath: '../js/ext/ux/img/',											    
											    multiselects: [
													{
														width: 155,
												        height: 125,
												        store: new Ext.data.JsonStore({
									                    	fields: [ 'text', 'value' ],
												            data: [
																//<s:iterator value="publicProjects">
																	{ text: '<s:property value="name" />', value: <s:property value="id" /> },
																//</s:iterator>
														    ]
												        }),
												        displayField: 'text',
												        valueField: 'value'
												    }, {
									                	width: 155,
									                	height: 125,
									                	store: new Ext.data.JsonStore({
									                    	fields: [ 'text', 'value' ],
												            data: []
									                	}),
									                	displayField: 'text',
												        valueField: 'text'																							                	
									            	}
									            ]
											}
										]
									}),
						            new Ext.Panel({
							            fieldLabel: '<s:text name="registration.form.captcha" />*',
							            items: [ 
									        new Ext.Panel({
							            		html: '<img title="Click to change" id="captcha" onclick="reloadCaptcha();" alt="<s:text name="registration.form.captcha" />" width="330" height="75" src="<s:url action="Captcha" namespace="/" />" />'
							            	}), {
												style: { marginTop: 5 },
												width: 330,
									        	xtype: 'textfield',
									        	name: "captcha",
									        	allowBlank: false,
									        	selectOnFocus: true
								        	}
							            ]						            	
						            })					            										
						        ],
						        buttonAlign: "center",
						        buttons: [{
							        id: "buttonSubscribe",
						            text: '<s:text name="registration.form.subscribe" />',
						            handler: function() {
							            if(Ext.getCmp('itemselector').getValue()=='') {
							            	Ext.MessageBox.alert('Status', "Select a project");
							            	return;
							            }
						            	Ext.getCmp("buttonSubscribe").setDisabled(true);
						            	Ext.getCmp("buttonClear").setDisabled(true);
						        		Ext.getCmp("registrationForm").getForm().submit({
						        			clientValidation: true,
							        		waitMsg: '<s:text name="registration.form.submitMsg" />',
							        		success: function(form, action) {
							        			Ext.getCmp("registrationForm").setDisabled(true);
							        			if(eval(action.response.responseText)) {
							        				Ext.MessageBox.alert('Status', 'Success!\nPlease check your e-mail to confirm your registration.');
							        				setTimeout('document.location=\'<s:url action="Home" namespace="/" encode="false" />\'', 5000);
							        			}
							        			else
							        				Ext.MessageBox.alert('Status', 'Error');
							        		},
							        		failure: function(form, action) {
							        			reloadCaptcha();
							        			Ext.getCmp("buttonSubscribe").setDisabled(false);
							        			Ext.getCmp("buttonClear").setDisabled(false);
						        				Ext.MessageBox.alert('Status', 'Error');							        			
							        		},
							        		params: {
							        			activationUrl: pathURL + 'secure/AccountActivation'
							        		},
							        		timeout: 60
							        	});
						        	}			            
						        }, {
							        id: "buttonClear",
						            text: '<s:text name="registration.form.clear" />',
						            handler: function() {
							            Ext.getCmp("registrationForm").getForm().reset();
							            Ext.getCmp("registrationForm").getForm().clearInvalid();
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
