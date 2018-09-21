thinwalletCtrls.controller('VersionCtrl', function ($scope, $http, ApiCalls) {

    $scope.last_git_commit_date = "";
    $scope.last_git_commit_hash = "";
    $scope.scp1310jp1_version_full = "";

    var version = ApiCalls.getVersion()
        .then(function(response) {

            var last_git_commit_date = response.data.last_git_commit_date;
            var last_git_commit_hash = response.data.last_git_commit_hash;
            var git_branch_name      = response.data.git_branch_name;
            var scp1310jp1_version_full  = response.data.scp1310jp1_version_full;

            // on the backend, api version is in uint32 format.
            var api_major = response.data.api >> 16;
            var api_minor = response.data.api & 0xffff;

            $scope.version = "Open Scp1310jp1 version (api): "
                + git_branch_name + "-" + last_git_commit_date + "-" + last_git_commit_hash
                + " (" + api_major + "." + api_minor + ")"
                + "  | Scp1310jp1 version: " + scp1310jp1_version_full
                + "  | Blockchain height: " + response.data.blockchain_height

        }, function(response) {
            $scope.version = "Error: Can't connect to the backend! Maybe it is down."
        });
});