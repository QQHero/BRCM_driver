(window.webpackJsonp=window.webpackJsonp||[]).push([["systemUpgrade"],{"0efa":function(e,t,s){"use strict";s("91f6")},"38b2":function(e,t,s){"use strict";s("7365")},"3f86":function(e,t,s){},6192:function(e,t,s){"use strict";s("e058")},7365:function(e,t,s){},"91f6":function(e,t,s){},afa1:function(e,t,s){"use strict";s.r(t);var i=s("710d"),a={props:{data:{type:Object,default:()=>({percent:1,onlineUpdateFail:!1,onlineUpdateSuccess:!1})}},data:()=>({timer:null}),mounted(){this.startTimer()},watch:{"data.onlineUpdateFail":{handler(e){e&&this.setFail()},immediate:!0},"data.onlineUpdateSuccess":{handler(e){e&&this.setSuccess()},immediate:!0},"data.percent":{handler(e){e<100?sessionStorage.setItem(this.data.sn,e):sessionStorage.removeItem(this.data.sn)},immediate:!0}},methods:{startTimer(){this.clearTimer(),this.timer=setInterval(()=>{let e=parseInt(10*Math.random());this.data.percent+=e,this.$emit("updateTableData",this.data),this.data.percent>=90&&this.clearTimer()},1e3)},setPercent(e){e>this.data.percent&&(this.data.percent=e,this.$emit("updateTableData",this.data))},clearTimer(){this.timer&&clearInterval(this.timer)},setSuccess(){this.data.percent=100,this.clearTimer(),this.$emit("updateTableData",this.data)},setFail(){this.data.percent=100,this.clearTimer(),this.$emit("updateTableData",this.data)}},destroyed(){this.clearTimer(),this.$emit("updateTableData",this.data)}},r=(s("38b2"),s("0b56")),o={components:{onlineProcess:Object(r.a)(a,(function render(){var e=this,t=e._self._c;return t("div",{staticClass:"process",class:{"process--success":e.data.onlineUpdateSuccess,"process--fail":e.data.onlineUpdateFail}},[t("div",{staticClass:"process__bar"},[t("div",{staticClass:"process__bar--active",style:"width:"+e.data.percent+"%"})]),e.data.onlineUpdateSuccess?t("i",{staticClass:"v-icon-ok-plane process__percent"}):e._e(),e.data.onlineUpdateFail?t("i",{staticClass:"v-icon-close-plane process__percent"}):e._e(),e.data.onlineUpdateFail||e.data.onlineUpdateSuccess?e._e():t("span",{staticClass:"process__percent"},[e._v(" "+e._s(e.data.percent)+"% ")])])}),[],!1,null,null,null).exports},props:{tableData:{type:Array,default:()=>[]}},data(){return this.CONFIG_MESH_SUPPORT="y"==window.CONFIG_MESH_SUPPORT,{lang:window.B.getLang()}},filters:{optionTexts:e=>e?_("Collapse"):_("G#VAF#Details")},methods:{changeVal(e){e.showTips=!e.showTips,this.$refs.table.updateTable(),this.updateTableData(e)},showError(e,t){e.error=t,setTimeout(()=>{e.error=""},5e3)},showDetection:e=>"normal"==e.nodeStatus&&"upgrading"!=e.lastStatus&&!(e.versionInfo&&e.versionInfo.hasNewSoftVersion),showOnline:e=>"normal"==e.nodeStatus&&"upgrading"!=e.lastStatus&&e.versionInfo&&e.versionInfo.hasNewSoftVersion,showOnlineProcess:e=>"upgrading"==e.nodeStatus||"upgrading"==e.lastStatus,onlineUpgrade(e){this.$emit("onlineUpgrade",e)},checkNew(e){this.$emit("checkNew",e)},localUpgrade(e){this.$emit("localUpgrade",e)},getCurrentOptionIndex(e){return this.tableData.findIndex(t=>t.sn==e.sn)},updateTableData(e){let t=this.getCurrentOptionIndex(e);this.tableData[t]=e},handleDescription(e){return(e[this.lang]||e.en).split("\\n")}}},n=(s("d79b"),Object(r.a)(o,(function render(){var e=this,t=e._self._c;return t("v-table",{ref:"table",attrs:{data:e.tableData,border:""}},[t("v-table-col",{attrs:{prop:"nodeName",label:e._("Device Name"),width:"353px",align:"center"},scopedSlots:e._u([{key:"default",fn:function(s){return[t("div",{staticClass:"dev-info"},[t("div",{staticClass:"dev-info-warp"},[t("span",{staticClass:"dev-info-warp__name"},[e._v(e._s(s.nodeName))]),"controller"==s.nodeType&&e.CONFIG_MESH_SUPPORT?t("span",{staticClass:"dev-info-warp__main"},[e._v(e._s(e._("Primary Node")))]):e._e()]),"detecting"===s.nodeStatus||s.error?e._e():t("div",{staticClass:"dev-info__version"},[s.versionInfo&&s.versionInfo.hasNewSoftVersion?t("div",{staticClass:"dev-info__new-version"},[t("div",{staticClass:"dev-info__new-version-title"},[t("span",{staticClass:"text-active"},[e._v(e._s(e._("New Version Available: %s",[s.versionInfo.updateContent])))]),t("v-button",{attrs:{type:"text"},on:{click:function(t){return e.changeVal(s)}}},[e._v(e._s(e._f("optionTexts")(s.showTips)))])],1),s.showTips?t("div",[t("ul",{staticClass:"dev-info__new-version-ul"},e._l(e.handleDescription(s.versionInfo.description),(function(s,i){return t("li",{key:i,staticClass:"dev-info__new-version-li"},[e._v(" "+e._s(s)+" ")])})),0)]):e._e()]):e._e(),s.versionInfo&&s.versionInfo.updateContent==s.currentVersion?t("div",{staticClass:"text-active"},[e._v(" "+e._s(e._("The router is up to date"))+" ")]):e._e()]),s.error?t("div",{staticClass:"error-text"},[e._v(e._s(s.error))]):e._e()])]}}])}),t("v-table-col",{attrs:{prop:"currentVersion",label:e._("Current Firmware Version"),width:"180px",align:"center"}}),t("v-table-col",{attrs:{label:e._("Operation"),align:"center"},scopedSlots:e._u([{key:"default",fn:function(s){return[t("div",{staticClass:"dev-options"},[t("div",{staticClass:"dev-options__online-warp"},[e.showOnline(s)?t("v-button",{attrs:{type:"secondary",size:"S"},on:{click:function(t){return e.onlineUpgrade(s)}}},[t("div",{staticClass:"dev-options__btn-text"},[e._v(e._s(e._("Online Upgrade")))])]):e._e(),e.showDetection(s)?t("v-button",{attrs:{type:"secondary",size:"S"},on:{click:function(t){return e.checkNew(s)}}},[t("div",{staticClass:"dev-options__btn-text"},[e._v(" "+e._s(e._("Detect New Version"))+" ")])]):e._e(),e.showOnlineProcess(s)?t("online-process",{attrs:{data:s},on:{updateTableData:e.updateTableData}}):e._e(),"detecting"==s.nodeStatus?t("span",[e._v(e._s(e._("Detecting…")))]):e._e()],1),t("v-button",{attrs:{size:"S",disabled:e.showOnlineProcess(s)},on:{click:function(t){return e.localUpgrade(s)}}},[t("div",{staticClass:"dev-options__btn-text"},[e._v(e._s(e._("Local Upgrade")))])])],1)]}}])})],1)}),[],!1,null,null,null).exports),l={props:{},data:()=>({ip:"",isConfirmLoading:!1,isMainNode:!0,formData:{sn:""},timer:null,showDialog:!1}),computed:{uploadUrl(){return this.isMainNode?"/cgi-bin/upgrade":"//"+this.ip+"/cgi-bin/upgrade"}},methods:{showModal(e,t,s){this.formData.sn=e,this.ip=t,this.isMainNode=!!s,this.showDialog=!0,this.isConfirmLoading=!1},hideModal(){this.showDialog=!1,this.clearTimer()},confirmUpload(){this.$refs.upload.submit()},cancelUpload(){this.$refs.upload.clearFile()},beforeUpload(e){return this.isConfirmLoading=!0,this.clearTimer(),e?/.bin$/.test(e)?this.$postModule({agentLocalUpgrade:{sn:this.formData.sn}},!1).then(e=>3==e.errCode?(this.$message.error(_("The current node is already in the uploaded software")),this.isConfirmLoading=!1,!1):4==e.errCode?(this.$emit("showProgressDialog",!0),!1):(this.timer=setTimeout(()=>{this.$message.error(_("Failed to upgrade")),this.hideModal()},6e4),!0)).catch(()=>(this.isConfirmLoading=!1,!1)):(this.$message.error(_("Please choose a BIN file")),this.isConfirmLoading=!1,!1):(this.$message.error(_("Please choose the upgrade file")),this.isConfirmLoading=!1,!1)},clearTimer(){this.timer&&clearTimeout(this.timer)},onSuccess(e){this.clearTimer();let t={1:_("Incorrect format"),2:_("CRC check failed"),3:_("Incorrect file size"),4:_("Failed to upgrade"),5:_("Insufficient memory. Please reboot the router before the upgrade")};null!=e.errCode&&(t[e.errCode]?(this.$message.error(t[e.errCode]),this.$emit("showProgressDialog",!1)):this.$emit("showProgressDialog",!0),this.showDialog=!1)}}},c=(s("6192"),Object(r.a)(l,(function render(){var e=this,t=e._self._c;return t("v-dialog",{attrs:{width:560,title:e._("Local Upgrade"),confirmButtonText:e._("Upgrade"),isLoading:e.isConfirmLoading},on:{confirm:e.confirmUpload,"after-close":e.clearTimer},model:{value:e.showDialog,callback:function(t){e.showDialog=t},expression:"showDialog"}},[t("div",{staticClass:"local-upgrade"},[t("span",{staticClass:"local-upgrade__icon v-icon-remind-line"}),t("div",{staticClass:"local-upgrade__title"},[e._v(" "+e._s(e._("The device will reboot after the upgrade completes. The whole process takes about 3 minutes. Continue?"))+" ")]),t("div",{staticClass:"local-upgrade__tips"},[e._v(" "+e._s(e._("The upgrade file is a BIN file"))+" ")]),t("v-upload",{ref:"upload",attrs:{data:e.formData,action:e.uploadUrl,accept:".bin","cross-domain":"","show-file-list":"",noFileText:e._("No file chosen"),beforeUpload:e.beforeUpload,onSuccess:e.onSuccess}})],1)])}),[],!1,null,null,null).exports),d=s("e117"),p=s("7187");let h={showTips:!1,onlineUpdateFail:!1,onlineUpdateSuccess:!1,percent:0};var u={components:{upgradeList:n,localUpgrade:c,progressBar:d.a},data(){return this.CONFIG_MESH_SUPPORT="y"==window.CONFIG_MESH_SUPPORT,{pageTitle:_("Firmware Upgrade"),pageTips:_("Through firmware upgrades, the router can get new functions or more stable performance"),isloading:!0,showProgress:!1,deviceVersionList:[],lastDevList:{},timer:null,isOneKeyDetecting:!0,isLocalSuccess:!0,progressSuccess:{title:_("Upgrading… Please wait"),tips:_("Do not power off the router or disconnect it from the internet during the upgrade; otherwise, the upgrade may fail or the router may be damaged.")},processError:{title:_("Upgrade failed. Rebooting…"),tips:_("")},localhostSn:""}},created(){this.updateData(),this.setTimer()},computed:{oneKeyText(){return this.isOneKeyDetecting?_("Check New Version"):_("One-click Upgrade")},progress(){return this.isLocalSuccess?this.progressSuccess:this.processError}},methods:{updateData(){this.$getData({modules:"deviceVersionList,localhost"}).then(e=>{this.localhostSn=e.localhost.sn,this.formatLastData(),this.formatData(e.deviceVersionList),this.deviceVersionList=Object(i.e)(e.deviceVersionList),this.isloading=!1})},formatLastData(){this.lastDevList={},this.deviceVersionList.forEach(e=>{let{showTips:t,percent:s,nodeStatus:i}={...e};this.lastDevList[e.sn]={showTips:t,percent:s,lastStatus:i}})},formatData(e){this.isOneKeyDetecting=!1,e.forEach((t,s)=>{e[s]=Object.assign(t,h,this.lastDevList[t.sn]||{}),e[s].percent=Number(sessionStorage.getItem(e[s].sn))||e[s].percent,this.lastDevList[t.sn]&&t.nodeStatus!=this.lastDevList[t.sn].lastStatus?("detecting"==this.lastDevList[t.sn].lastStatus&&p.d[t.versionInfo.verErrCode]&&(t.error=p.d[t.versionInfo.verErrCode]),"upgrading"==this.lastDevList[t.sn].lastStatus&&(p.d[t.upgradeInfo.upErrCode]?(t.error=p.d[t.upgradeInfo.upErrCode],t.onlineUpdateFail=!0):t.onlineUpdateSuccess=!0)):"upgrading"==t.nodeStatus&&"4"==t.upgradeInfo.upgradeStatus&&(t.onlineUpdateSuccess=!0,t.sn!=this.localhostSn&&"controller"!=t.nodeType||this.showProgressDialog(!0)),"normal"!=t.nodeStatus&&"detecting"!=t.nodeStatus||t.versionInfo&&t.versionInfo.hasNewSoftVersion||(this.isOneKeyDetecting=!0)})},setTimer(){this.clearTimer(),this.timer=setInterval(()=>{this.updateData()},5e3)},clearTimer(){this.timer&&clearInterval(this.timer)},checkNew(e){let t={deviceVersionList:{action:"queryversion"}};e&&(t.deviceVersionList.sn=e.sn),this.deviceVersionList.forEach(t=>{e&&e.sn!=t.sn||(t.nodeStatus="detecting")}),this.$postModule(t,!1).then(()=>{this.updateData()})},onlineUpgrade(e){let t={deviceVersionList:{action:"queryupgrade"}};e&&(t.deviceVersionList.sn=e.sn),this.deviceVersionList.forEach(e=>{e.percent=0,sessionStorage.removeItem(e.sn)}),this.$postModule(t,!1).then(()=>{this.updateData()})},localUpgrade(e){this.$refs.localUpdate.showModal(e.sn,e.ip,"controller"==e.nodeType)},oneKeyClick(){this.isOneKeyDetecting?this.checkNew():this.onlineUpgrade()},showProgressDialog(e){this.isLocalSuccess=e,this.showProgress=!0,this.clearTimer()}},destroyed(){this.clearTimer()}},g=(s("0efa"),Object(r.a)(u,(function render(){var e=this,t=e._self._c;return t("div",{staticClass:"upgrade-set"},[t("v-page-title",{attrs:{title:e.pageTitle,tips:e.pageTips}}),t("upgrade-list",{attrs:{tableData:e.deviceVersionList,isLoading:e.isloading},on:{checkNew:e.checkNew,onlineUpgrade:e.onlineUpgrade,localUpgrade:e.localUpgrade}}),e.CONFIG_MESH_SUPPORT?t("div",{staticClass:"upgrade-set__btn"},[t("v-button",{attrs:{type:"primary"},on:{click:e.oneKeyClick}},[e._v(" "+e._s(e.oneKeyText)+" ")])],1):e._e(),t("local-upgrade",{ref:"localUpdate",on:{showProgressDialog:e.showProgressDialog}}),e.showProgress?t("progress-bar",{attrs:{time:1600,title:e.progress.title,tips:e.progress.tips}}):e._e()],1)}),[],!1,null,null,null));t.default=g.exports},d79b:function(e,t,s){"use strict";s("3f86")},e058:function(e,t,s){}}]);