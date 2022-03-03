#[derive(Debug)]
pub enum ServiceError {

}

#[derive(Debug)]
pub enum SecurityError {
    IdentityAlreadyCreated,
    PermissionDeniedOnKeyPath,
}

#[derive(Debug)]
pub enum Error {
    Service(ServiceError),
    Security(SecurityError),
}