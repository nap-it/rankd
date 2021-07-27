use warp::Filter;
use slog::{Logger, info};
use json::object;
use crate::node::System;
use std::net::Ipv4Addr;

pub async fn start(log: Logger, ip: Ipv4Addr) {
    info!(log, "Successfully loaded system information.");

    let testing_tool = warp::path::end().map(|| {
        format!("Welcome to this service")
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

    let userspace_info = warp::path("user").map(|| {
        format!("{:#?}", crate::capabilities::Capabilities::user())
    });

    let network_info = warp::path("network").map(|| {
        format!("{:#?}", crate::capabilities::Capabilities::user().network())
    });

    let network_interface_info = warp::path!("networks" / String).map(|interface| {
         format!("{:#?}", crate::capabilities::Capabilities::user().network_interface(interface).unwrap())
    });

    let routes = warp::get().and(
        kernel_info
            .or(userspace_info)
            .or(network_info)
            .or(network_interface_info)
    );

    info!(log, "Running server in http://{}:{}.", ip, 7265);
    warp::serve(routes).run((ip.octets(), 7265)).await;
}