<?php
$key_store = array();
$client_id  = 'IUSqVRmBazJQlII';
$server_id  = 'SRooyjxtvWYUmXe';
$server_key = 'VUVDMgAAAC0s7a3tA8fDqH3oz5dalSfz0zodqZWh63P4cVIUESVIK904PJVX';

function get_pub_key_by_id($id)
    {
    global $key_store;
    $pubkey='';
    if(!empty($key_store[$id]))
	{
	$pubkey=$key_store[$id]['public_key'];
	}
    return($pubkey);
    }

function do_curl($context, $url, $hdrs=array(), $doc='')
    {
    curl_setopt($context, CURLOPT_URL,$url);
    if (!empty($hdrs))
	curl_setopt($context, CURLOPT_HTTPHEADER,$hdrs);
    curl_setopt($context, CURLOPT_SSL_VERIFYHOST,  0);
    curl_setopt($context, CURLOPT_USERAGENT, "Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)");
    curl_setopt($context, CURLOPT_FAILONERROR, 1);
    curl_setopt($context, CURLOPT_TIMEOUT, 900);
    curl_setopt($context, CURLOPT_RETURNTRANSFER,1);
    curl_setopt($context, CURLOPT_FOLLOWLOCATION,1);
    curl_setopt($context, CURLOPT_NOBODY,0);
    curl_setopt($context, CURLOPT_HEADER,1);
    //curl_setopt($context, CURLINFO_HEADER_OUT,1);
    if (!empty($doc))
	{
	//curl_setopt($context, CURLOPT_POST,1);
	curl_setopt($context, CURLOPT_CUSTOMREQUEST,'POST');
        curl_setopt($context, CURLOPT_POSTFIELDS, $doc);
	}
    
    $content=curl_exec($context);
    $err=curl_errno($context);
    $info = curl_getinfo($context);
    $get_return_number = curl_getinfo($context, CURLINFO_HTTP_CODE);

    $response=array();
    $response['curl_err']=$err;
    $response['curl_ret']=$get_return_number;
    $content=explode("\r\n\r\n",$content,2);
    $response['curl_headers']=$content[0];
    $response['curl_content']=@$content[1];
    $response['curl_info']=$info;
    return($response);
    }

function themis_ci_message($client_id, $server_id, $server_key)
    {
    global $key_store;
    
    $endpoint   = 'https://themis.cossacklabs.com/api/'.$client_id.'/';
    $key_store[$server_id]['public_key'] =base64_decode($server_key);
    $key_store[$client_id]['public_key'] =base64_decode('VUVDMgAAAC2IRalnA7mIDaKlK5HvBz2woET8cc2dJQ3sOQMYQ/13TtUNofnQ');
    $key_store[$client_id]['private_key']=base64_decode('UkVDMgAAAC1whm6SAJ7vIP18Kq5QXgLd413DMjnb6Z5jAeiRgUeekMqMC0+x');
    
    $headers = array();
    $headers[]='Content-type: application/x-www-form-urlencoded';
    
    $message = 'The best laid plans of mice and men go oft astray';
    $message = base64_encode(phpthemis_secure_message_wrap($key_store[$client_id]['private_key'],$key_store[$server_id]['public_key'],$message));
    $docbody = 'message='.urlencode($message);
    
    $curl_ctx = curl_init();

    $response = do_curl($curl_ctx,$endpoint,$headers,$docbody);
    $message=$response['curl_content'];
    $message=phpthemis_secure_message_unwrap($key_store[$client_id]['private_key'],$key_store[$server_id]['public_key'],$message); 
    echo $message."\n";
    
    curl_close ($curl_ctx);
    }

themis_ci_message($client_id, $server_id, $server_key);

?>