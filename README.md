**STUDENT INFO**

* **Full Name**: Jonas Ooms
* **Github Account**: JonasOoms
* **Github repository link**: [<url_to_this_repository>](https://github.com/Howest-DAE-GD/gameplay-programming-framework-JonasOoms/tree/developer)

---

**EXTRA ASSIGNMENT**
**Week**: Week 3
**Extra Assignment**: Using index calculations to calculate the rows and columns of the CellSpace inside the neighborhood radius

---

**INFO FOR STUDENTS**

These are the start files for Gameplay Programing, based on the template used for the Github Classroom.

There are different branches available, namely 'main', '**developer**', 'hotfix' and a branch with the start files of every week where the name of the branch starts with the week name, for example 'W1_SteeringBehaviors'. The following rules are very important to guarantuee correct behavior:
* Do **NOT** work in either the 'main', 'hotfix' or week branches!
* Always work in the 'developer' branch **or** use your own branches (which you ideally merge into the 'developer' branch)
* If a hotfix would be needed, given the limitations of classrooms, these hotfixes will be pushed into the 'hotfix' branch. Any changes you might have in that branch (which you shouldn't have) would be lost! Hence, do not work inside this branch. If a hotfix would occur we will let you know how to integrate it into your repository. Alternatively we might share new code via Leho/Canvas if we deem so.

While you have access to all start files, refrain yourself from looking at them as it might seem very complicated without the theory. Don't worry, we will explain everything. Hint: there are not solutions on there, even if we a certain week's content relies on a solution, you will have to provide the solution yourself from your repository. Some weeks might give merge conflicts given we had to inject some dummy code. So read the comment below.

If you merge or rebase the 'developer' branch onto a new week's starter branch, there might be conflicts. These can arise from dummy code or because you changed things in sources that might have changed. The latter shouldn't happen often as we usually provide new starter code per week. In this case, don't just resolve without checking what is wrong. Always inspect which file and which code gets affected. If you would have questions or are not sure about a merge conflict, ask the teachers **during class**!

How to use this repostory with git?
* The very first time, you should setup your git by following the Software Engineering class about git. Please use VSCode for merge conflicts as this is one of the best tools out there.
* Every week you take your 'developer' branch and first commit and push your solutions to the remote repository (see Software Engineering class). Even if you are not done, please do this! You should submit code to your repository every week!
* Once your code is on the remote, you can safely take the new week's content by **rebasing** or **merging** that branch into your 'developer' branch (see Software Engineering class) using `git rebase <branch_name>` or `git rebase <branch_name>` respectively (replace '<branch_name>' with the actual branch name).
* Verify you have the changes from the new branch. You can now start working on the new content. If you want you can already push these changes to the remote, but this isn't necessary.

You will notice that every week you get new files, but two existing files will chance, namely **App_Selector.h** and the **CMakelists.txt** files. This is because we add the new files to CMake and also set the new application of the lab active. Feel free to toggle between application in the **App_Selector.h**. If you added your own files in the **CMakelists.txt** file, again be mindfull with merge conflicts.

Every week we will make new start files available my merging them into the main branch. At the beginning of the class, pull in the new changes and rebase/merge your current code to those changes. Merge conflicts might arise if you have changed some base files, in this case you will need to resolve the conflicts yourself.
When pulling in the code, you will have merge conflict with the **App_Selector.h** and the **CMakelists.txt** file. In this case, either use the recent version or when you added extra code yourself, do a manual merge.
 
Handing in milestones happens on Leho/Canvas. Just download a .zip of the repository and hand that in. Rename the downloaded .zip so it contains your name. Also make sure this ReadMe.md file is up to date with your student info, so we can easily check your repository from the downloaded .zip file.

If you have any questions, contact the teachers of this course!

-- GPP TEAM --
