var bootstrapCss = 'bootstrapCss';
if (!document.getElementById(bootstrapCss)) {
	var head = document.getElementsByTagName('head')[0];
	var bootstrapWrapper = document.createElement('link');
	bootstrapWrapper.id = bootstrapCss;
	bootstrapWrapper.rel = 'stylesheet/less';
	bootstrapWrapper.type = 'text/css';
	bootstrapWrapper.href = intel_admin_settings.intel_url + 'admin/css/intel-bootstrap-wrapper.less';
	bootstrapWrapper.media = 'all';
	head.appendChild(bootstrapWrapper);


	var lessjs = document.createElement('script');
	lessjs.type = 'text/javascript';
	lessjs.src = intel_admin_settings.intel_url + 'admin/js/less.min.js';
	head.appendChild(lessjs);

	//load other stylesheets that override bootstrap styles here, using the same technique from above

	var customStyles = document.createElement('link');
	customStyles.id = "customStyles";
	customStyles.rel = 'stylesheet';
	customStyles.type = 'text/css';
	customStyles.href = intel_admin_settings.intel_url + 'admin/css/intel-admin.css';
	customStyles.media = 'all';
	head.appendChild(customStyles);
}