(window.webpackJsonp=window.webpackJsonp||[]).push([["wifiMesh"],{"25e2":function(t,e,n){"use strict";n.r(e);var i={data:()=>({ruleForm:{meshBtnEn:!1},isLoading:!0}),computed:{isWifi:()=>"n"===CONFIG_WEB_MESH_BUTTON,pageTips(){return this.isWifi?"":_("The router has a MESH button and can form a network with other Tenda NOVA which also have a MESH button.")}},created(){this.getData()},methods:{getData(){this.$getData({modules:"meshBtn"}).then(t=>{this.isLoading=!1,this.ruleForm.meshBtnEn=t.meshBtn.meshBtnEn})},changeEn(){this.$postModule({meshBtn:{meshBtnEn:this.ruleForm.meshBtnEn}}).then(t=>{0==+t.errCode&&this.getData()})}}},o=n("0b56"),s=Object(o.a)(i,(function render(){var t=this,e=t._self._c;return e("div",[e("v-loading",{attrs:{visible:t.isLoading}}),t.isLoading?t._e():e("v-page",{ref:"page",attrs:{"show-footer":!1}},[e("v-page-title",{attrs:{title:t.isWifi?t._("MESH"):t._("MESH Button"),tips:t.pageTips}}),e("v-form",{attrs:{name:"meshBtn"}},[e("v-form-item",{attrs:{label:t.isWifi?t._("MESH"):t._("MESH Button"),prop:"meshBtnEn"}},[e("v-switch",{on:{change:t.changeEn},model:{value:t.ruleForm.meshBtnEn,callback:function(e){t.$set(t.ruleForm,"meshBtnEn",e)},expression:"ruleForm.meshBtnEn"}}),e("div",{staticClass:"help-text"},[e("p",[t._v(t._s(t._("Note:")))]),t.isWifi?t._e():e("p",[t._v(" "+t._s(t._("1. For information security, do not toggle on MESH Button when using the router in public areas."))+" ")]),t.isWifi?t._e():e("p",[t._v(" "+t._s(t._("2. With this function disabled, you cannot form network by using the MESH button on the device. However, you can use the Tenda WiFi app or web UI to add the device to a network."))+" ")]),t.isWifi?e("p",[t._v(" "+t._s(t._("1. After this function is enabled, you can use the WPS button on the router to perform MESH networking with other network devices."))+" ")]):t._e(),t.isWifi?e("p",[t._v(" "+t._s(t._("2. After this function is disabled, you cannot use the WPS button on the router for networking. You can use Tenda WiFi App or Web UI configuration to form a MESH network with other devices."))+" ")]):t._e()])],1)],1)],1)],1)}),[],!1,null,null,null);e.default=s.exports}}]);