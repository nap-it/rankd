use warp::Filter;
use slog::{Logger, info};
use json::object;
use crate::node::System;
use std::net::Ipv4Addr;
use crate::security::get_own_identifier;
use std::collections::HashMap;

pub async fn start(log: Logger, ip: Ipv4Addr, installation_path: String) {
    info!(log, "Successfully loaded system information.");

    let testing_tool = warp::path::end().map(|| {
        format!("Welcome to this service")
    });

    // Registration services.

    let hash_request_info = log.clone();
    let hash_request = warp::path("identification").map(move || {
        info!(hash_request_info, "Received request in /identification endpoint.");
        let data = get_own_identifier(installation_path.clone()).unwrap();
        format!("{}", object!{
            code : 200,
            data : data,
        })
    });

    let receive_token_log = log.clone();
    let receive_registration = warp::path("register").and(warp::query::<HashMap<String, String>>()).map(move |parameter: HashMap<String, String>| {
        let mut user_access_token = String::new();

        match parameter.get("token") {
            None => {}
            Some(token) => user_access_token = token.parse().unwrap(),
        }

        info!(receive_token_log, "The user has written {} as token", user_access_token);

        // TODO  connect with Auth0 to verify if the access-token is valid or not.

        format!("{}", object!{
            code : 200,
            data : "a",
        })
    });

    let log_kernel_info = log.clone();
    let kernel_info = warp::path("os").map(move || {
        info!(log_kernel_info, "Received request in /os endpoint.");
        let data = System::global();
        format!("{}", object!{
            code : 200,
            data : data,
        })
    });

    let routes = warp::get().and(
        kernel_info
            .or(hash_request)
            .or(receive_registration)
    );

    info!(log, "Running server in http://{}:{}.", ip, 7265);
    warp::serve(routes).run((ip.octets(), 7265)).await;
}